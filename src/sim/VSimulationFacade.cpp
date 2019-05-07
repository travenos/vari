/**
 * Project VARI
 * @author Alexey Barashkov
 */
#ifdef DEBUG_MODE
#include <QDebug>
#endif

#include <Inventor/Qt/SoQt.h>
#include <QFile>
#include <QSettings>

#include "VSimulationFacade.h"
#include "layer_builders/VLayerFromGmeshBuilder.h"
#include "layer_builders/VLayerFromAnsysBuilder.h"
#include "layer_builders/VLayerManualBuilder.h"
#include "layer_builders/VLayerFromLayerBuilder.h"
#include "import_export/VModelExport.h"
#include "import_export/VModelImport.h"
#include "structures/VExceptions.h"
#include "structures/VTable.h"

/**
 * VSimulationFacade implementation
 */


VSimulationFacade::VSimulationFacade(QWidget *parent):
    m_parentWidget(parent),
    m_pSimulator(new VSimulator),
    m_pLayersProcessor(new VLayersProcessor),
    m_selectInjectionPoint(false),
    m_selectVacuumPoint(false),
    m_pInjectionVacuum(new VInjectionVacuum),
    m_pTable(new VTable),
    m_useTableParameters(true),
    m_pNodesLock(new std::mutex),
    m_pTrianglesLock(new std::mutex)
{
    SoQt::init(parent);
    m_pGraphicsViewer.reset(new VGraphicsViewer(parent, m_pSimulator));
    m_pSimulator->setMutexes(m_pNodesLock, m_pTrianglesLock);
    m_pGraphicsViewer->setMutexes(m_pNodesLock, m_pTrianglesLock);
    connectMainSignals();
    initLayersProcessor();
}

VSimulationFacade::~VSimulationFacade()
{
    m_pGraphicsViewer.reset();
    m_pLayersProcessor.reset();
    m_pSimulator.reset();
}

void VSimulationFacade::connectMainSignals()
{
    connect(m_pGraphicsViewer.get(),
            SIGNAL(gotNodesSelection(const VGraphicsViewer::const_uint_vect_ptr &)),
            this,
            SLOT(m_on_got_nodes_selection(const VGraphicsViewer::const_uint_vect_ptr &)));
    connect(m_pGraphicsViewer.get(), SIGNAL(gotPoint(const QVector3D &)),
            this, SLOT(m_on_got_point(const QVector3D &)));
    connect(m_pGraphicsViewer.get(), SIGNAL(gotTransformation()),
            this, SIGNAL(gotTransformation()));
    connect(m_pGraphicsViewer.get(), SIGNAL(cubeSideChanged(float)),
            this, SIGNAL(cubeSideChanged(float)));
    connect(m_pGraphicsViewer.get(), SIGNAL(askForDisplayingInfo()),
            this, SIGNAL(gotNewInfo()));
    connect(m_pGraphicsViewer.get(), SIGNAL(selectionEnabled(bool)),
            this, SIGNAL(selectionEnabled(bool)));
    connect(m_pGraphicsViewer.get(), SIGNAL(canceledDrag()),
            this, SLOT(updateGraphicsPositions()));
    connect(m_pGraphicsViewer.get(), SIGNAL(additionalControlsEnabled(bool)),
            this, SIGNAL(additionalControlsEnabled(bool)));

    connect(m_pSimulator.get(), SIGNAL(simulationStarted()),
            this, SIGNAL(simulationStarted()));
    connect(m_pSimulator.get(), SIGNAL(simulationPaused()),
            this, SIGNAL(simulationPaused()));
    connect(m_pSimulator.get(), SIGNAL(simulationStopped()),
            this, SIGNAL(simulationStopped()));

    connect(m_pSimulator.get(), SIGNAL(resinChanged()),
            this, SIGNAL(resinChanged()));

    connect(m_pSimulator.get(), SIGNAL(injectionDiameterSet(float)),
            this, SIGNAL(injectionDiameterSet(float)));
    connect(m_pSimulator.get(), SIGNAL(vacuumDiameterSet(float)),
            this, SIGNAL(vacuumDiameterSet(float)));
    connect(m_pSimulator.get(), SIGNAL(temperatureSet(double)),
            this, SIGNAL(temperatureSet(double)));
    connect(m_pSimulator.get(), SIGNAL(vacuumPressureSet(double)),
            this, SIGNAL(vacuumPressureSet(double)));
    connect(m_pSimulator.get(), SIGNAL(injectionPressureSet(double)),
            this, SIGNAL(injectionPressureSet(double)));
    connect(m_pSimulator.get(), SIGNAL(coefQSet(double)),
            this, SIGNAL(coefQSet(double)));
    connect(m_pSimulator.get(), SIGNAL(coefRSet(double)),
            this, SIGNAL(coefRSet(double)));
    connect(m_pSimulator.get(), SIGNAL(coefSSet(double)),
            this, SIGNAL(coefSSet(double)));
    connect(m_pSimulator.get(), SIGNAL(timeLimitSet(double)),
            this, SIGNAL(timeLimitSet(double)));
    connect(m_pSimulator.get(), SIGNAL(timeLimitModeSwitched(bool)),
            this, SIGNAL(timeLimitModeSwitched(bool)));
    connect(m_pSimulator.get(), SIGNAL(lifetimeConsiderationSwitched(bool)),
            this, SIGNAL(lifetimeConsiderationSwitched(bool)));
    connect(m_pSimulator.get(), SIGNAL(stopOnVacuumFullSwitched(bool)),
            this, SIGNAL(stopOnVacuumFullSwitched(bool)));

    connect(m_pSimulator.get(), SIGNAL(simulationStarted()),
            this, SLOT(disableInteraction()));
    connect(m_pSimulator.get(), SIGNAL(simulationPaused()),
            this, SLOT(disableInteraction()));
    connect(m_pSimulator.get(), SIGNAL(simulationStopped()),
            this, SLOT(enableInteraction()));
    connect(this, SIGNAL(canceledWaitingForInjectionPoint()),
            this, SLOT(enableInteraction()));
    connect(this, SIGNAL(canceledWaitingForVacuumPoint()),
            this, SLOT(enableInteraction()));
    connect(this, SIGNAL(injectionPointSet()),
            this, SLOT(enableInteraction()));
    connect(this, SIGNAL(vacuumPointSet()),
            this, SLOT(enableInteraction()));
}

void VSimulationFacade::initLayersProcessor()
{
    m_pLayersProcessor->setMutexes(m_pNodesLock, m_pTrianglesLock);
    connect(m_pLayersProcessor.get(), SIGNAL(layerVisibilityChanged(uint, bool)),
            this, SIGNAL(layerVisibilityChanged(uint, bool)));
    connect(m_pLayersProcessor.get(), SIGNAL(layerEnabled(uint, bool)),
            this, SIGNAL(layerEnabled(uint, bool)));
    connect(m_pLayersProcessor.get(), SIGNAL(layerRemoved(uint)),
            this, SIGNAL(layerRemoved(uint)));
    connect(m_pLayersProcessor.get(), SIGNAL(materialChanged(uint)),
            this, SIGNAL(materialChanged(uint)));
    connect(m_pLayersProcessor.get(), SIGNAL(nameChanged(uint)),
            this, SIGNAL(layerNameChanged(uint)));
    connect(m_pLayersProcessor.get(), SIGNAL(layerAdded()),
            this, SIGNAL(layerAdded()));
    connect(m_pLayersProcessor.get(), SIGNAL(layersCleared()),
            this, SIGNAL(layersCleared()));
}

void VSimulationFacade::startSimulation() 
{
    if (!m_pLayersProcessor->areLayersConnected())
        m_pLayersProcessor->createConnections();
    if (!m_pSimulator->isPaused())
        applyInjectionAndVacuumPoints();
    m_pSimulator->start();
}

void VSimulationFacade::stopSimulation() 
{
    m_pSimulator->stop();
}

void VSimulationFacade::pauseSimulation()
{
    m_pSimulator->pause();
}

void VSimulationFacade::resetSimulation() 
{
    m_pSimulator->reset();
    m_pGraphicsViewer->updateColors();
}

size_t VSimulationFacade::getLayersNumber() const
{
    return m_pLayersProcessor->getLayersNumber();
}

size_t VSimulationFacade::getActiveLayersNumber() const
{
    return m_pLayersProcessor->getActiveLayersNumber();
}

size_t VSimulationFacade::getInactiveLayersNumber() const
{
    return m_pLayersProcessor->getInactiveLayersNumber();
}

size_t VSimulationFacade::getNodesNumber() const
{
    return m_pLayersProcessor->getActiveNodesNumber();
}

size_t VSimulationFacade::getTrianglesNumber() const
{
    return m_pLayersProcessor->getActiveTrianglesNumber();
}

void VSimulationFacade::getModelSize(QVector3D &size) const
{
    m_pLayersProcessor->getActiveModelSize(size);
}

void VSimulationFacade::setVisible(uint layer, bool visible)
{
    m_pLayersProcessor->setVisibleLayer(layer, visible);
    m_pGraphicsViewer->updateVisibility(m_pLayersProcessor->getLayerId(layer));
}

void VSimulationFacade::removeLayer(uint layer)
{
    m_pLayersProcessor->removeLayer(layer);
    updateConfiguration();
}

void VSimulationFacade::enableLayer(uint layer, bool enable)
{
    m_pLayersProcessor->enableLayer(layer, enable);
    updateConfiguration();
}

void VSimulationFacade::moveLayerUp(uint layer)
{
    if(layer > 0)
    {
        m_pLayersProcessor->moveUp(layer);
        m_pGraphicsViewer->updatePosition(m_pLayersProcessor->getLayerId(layer));
        m_pGraphicsViewer->updatePosition(m_pLayersProcessor->getLayerId(layer - 1u));
        emit layersSwapped(layer, layer - 1u);
        emit configUpdated();
    }
}

void VSimulationFacade::moveLayerDown(uint layer)
{
    if(static_cast<int>(layer) < static_cast<int>(getLayersNumber()) - 1)
    {
        m_pLayersProcessor->moveDown(layer);
        m_pGraphicsViewer->updatePosition(m_pLayersProcessor->getLayerId(layer));
        m_pGraphicsViewer->updatePosition(m_pLayersProcessor->getLayerId(layer + 1u));
        emit layersSwapped(layer, layer + 1u);
        emit configUpdated();
    }
}

void VSimulationFacade::sortLayers()
{
    m_pLayersProcessor->sort();
    m_pGraphicsViewer->updatePositions();
    emit configUpdated();
}

void VSimulationFacade::setMaterial(uint layer, const VCloth &material)
{
    m_pLayersProcessor->setMaterial(layer, material);
    uint id = m_pLayersProcessor->getLayerId(layer);
    m_pGraphicsViewer->updatePositions();
    m_pGraphicsViewer->updateColors(id);
    emit configUpdated();
}

void VSimulationFacade::setResin(const VResin& resin)
{
    m_pSimulator->setResin(resin);
}

void VSimulationFacade::setInjectionPressure(double pressure) 
{
    m_pSimulator->setInjectionPressure(pressure);
}

void VSimulationFacade::setVacuumPressure(double pressure) 
{
    m_pSimulator->setVacuumPressure(pressure);
}


void VSimulationFacade::setTemperature(double temperature) 
{
    m_pSimulator->setTemperature(temperature);
}

void VSimulationFacade::setQ(double q) 
{
    m_pSimulator->setQ(q);
}
void VSimulationFacade::setR(double r) 
{
    m_pSimulator->setR(r);
}
void VSimulationFacade::setS(double s) 
{
    m_pSimulator->setS(s);
}

void VSimulationFacade::setTimeLimit(double limit)
{
    m_pSimulator->setTimeLimit(limit);
}

void VSimulationFacade::setTimeLimitMode(bool on)
{
    m_pSimulator->setTimeLimitMode(on);
}

void VSimulationFacade::considerLifetime(bool on)
{
    m_pSimulator->considerLifetime(on);
}

void VSimulationFacade::stopOnVacuumFull(bool on)
{
    m_pSimulator->stopOnVacuumFull(on);
}

void VSimulationFacade::enableAdditionalGraphicsControls(bool on)
{
    m_pGraphicsViewer->enableAdditionalControls(on);
}

void VSimulationFacade::newModel() 
{
    m_pSimulator->clear();
    m_pLayersProcessor->clear();
    m_pGraphicsViewer->clearAll();
    emit filenameChanged("");
}

void VSimulationFacade::loadModel(const QString &filename)
{
    m_pSimulator->pause();
    VModelImport loader;
    loader.loadFromFile(filename);
    m_pLayersProcessor = loader.getLayersProcessor();
    initLayersProcessor();
    updateConfiguration();
    m_pSimulator->setSimulationParameters(*(loader.getInfo()), *(loader.getSimulationParameters()),
                                          loader.getPaused(), loader.getTimeLimited());
    m_pInjectionVacuum = loader.getInjectionVacuum();
    useTableParameters(loader.getUseTableParameters());
    m_pGraphicsViewer->viewFromAbove();
    emit modelLoaded();
    emit filenameChanged(filename);
}

void VSimulationFacade::saveModel(const QString &filename)
{
    m_pSimulator->pause();
    VSimulationInfo info = m_pSimulator->getSimulationInfo();
    VSimulationParameters param = m_pSimulator->getSimulationParameters();
    bool paused = m_pSimulator->isPaused();
    bool timeLimited = m_pSimulator->isTimeLimitModeOn();
    VModelExport saver(info, param, *m_pInjectionVacuum,
                       m_pLayersProcessor, m_useTableParameters, paused, timeLimited);
    saver.saveToFile(filename);
    emit modelSaved();
    emit filenameChanged(filename);
}

VCloth::const_ptr VSimulationFacade::getMaterial(uint layer) const
{
    return m_pLayersProcessor->getMaterial(layer);
}

VSimulationParameters VSimulationFacade::getParameters() const
{
    return m_pSimulator->getSimulationParameters();
}

bool VSimulationFacade::isLayerVisible(uint layer) const
{
    return m_pLayersProcessor->isLayerVisible(layer);
}

bool VSimulationFacade::isLayerEnabled(uint layer) const
{
    return m_pLayersProcessor->isLayerEnabled(layer);
}

void VSimulationFacade::newLayerFromFile(const VCloth &material, const QString &filename,
                                         const QString &layerName,
                                         VLayerAbstractBuilder::VUnit units)
{
    QFile file(filename);
    VLayerFromFileBuilder * p_layerBuilder = nullptr;
    if (file.open(QIODevice::ReadOnly))
    {
        while (!file.atEnd())
        {
            QString line = file.readLine();
            if(line.contains("$MeshFormat",Qt::CaseInsensitive))
            {
                p_layerBuilder = new VLayerFromGmeshBuilder(filename,
                                                            material,                                                            
                                                            units);
                break;
            }
            else if(line.contains("ANSYS",Qt::CaseInsensitive))
            {
                p_layerBuilder = new VLayerFromAnsysBuilder(filename,
                                                            material,
                                                            units);
                break;
            }
        }
        file.close();
    }
    if (p_layerBuilder != nullptr)
    {
        try
        {
            p_layerBuilder->setLayerName(layerName);
            m_pLayersProcessor->addLayer(p_layerBuilder);
            delete p_layerBuilder;
            updateConfiguration();
            m_pGraphicsViewer->viewFromAbove();
        }
        catch(VImportException &e)
        {
            delete p_layerBuilder;
            throw e;
        }
    }
    else
        throw VImportException();
}

void VSimulationFacade::newLayerFromPolygon(const VCloth &material, const QPolygonF &polygon,
                                            double characteristicLength,
                                            const QString &layerName)
{
    VLayerManualBuilder * p_layerBuilder;
    p_layerBuilder = new VLayerManualBuilder(polygon, material, characteristicLength);
    p_layerBuilder->setLayerName(layerName);
    try
    {
        m_pLayersProcessor->addLayer(p_layerBuilder);
        delete p_layerBuilder;
        updateConfiguration();
        m_pGraphicsViewer->viewFromAbove();
    }
    catch(VImportException &e)
    {
        delete p_layerBuilder;
        throw e;
    }
}

void VSimulationFacade::duplicateLayer(uint layer)
{
    VLayer::const_ptr sourceLayer{m_pLayersProcessor->getLayer(layer)};
    if (sourceLayer && isSimulationStopped())
    {
        VLayerFromLayerBuilder layerBuilder(sourceLayer);
        m_pLayersProcessor->addLayer(&layerBuilder);
        updateConfiguration();
        m_pGraphicsViewer->viewFromAbove();
    }
    else
        throw VImportException();
}

void VSimulationFacade::updateConfiguration() 
{
    m_pSimulator->setActiveElements(m_pLayersProcessor->getActiveNodes(),
                                    m_pLayersProcessor->getActiveTriangles());
    m_pGraphicsViewer->setGraphicsElements(m_pLayersProcessor->getLayers());
    emit configUpdated();
}

void VSimulationFacade::updateGraphicsPositions()
{
    m_pGraphicsViewer->updatePositions();
}

void VSimulationFacade::waitForInjectionPointSelection(float diameter)
{
    cancelCuttingLayer();
    cancelDrag();
    cancelWaitingForVacuumPointSelection();
    m_pGraphicsViewer->enableDrag(false);
    m_selectInjectionPoint = true;
    m_selectVacuumPoint = false;
    m_pInjectionVacuum->injectionDiameter = diameter;
    m_pGraphicsViewer->viewFromAbove();
    m_pGraphicsViewer->setViewing(false);
    emit startedWaitingForInjectionPoint();
}

void VSimulationFacade::waitForVacuumPointSelection(float diameter)
{
    cancelCuttingLayer();
    cancelDrag();
    cancelWaitingForInjectionPointSelection();
    m_pGraphicsViewer->enableDrag(false);
    m_selectVacuumPoint = true;
    m_pInjectionVacuum->vacuumDiameter = diameter;
    m_pGraphicsViewer->viewFromAbove();
    m_pGraphicsViewer->setViewing(false);
    emit startedWaitingForVacuumPoint();
}

void VSimulationFacade::cancelWaitingForInjectionPointSelection()
{
    m_selectInjectionPoint = false;
    emit canceledWaitingForInjectionPoint();
}

void VSimulationFacade::cancelWaitingForVacuumPointSelection()
{
    m_selectVacuumPoint = false;
    emit canceledWaitingForVacuumPoint();
}

void VSimulationFacade::startCuttingLayer(uint layer)
{
    cancelDrag();
    cancelWaitingForVacuumPointSelection();
    cancelWaitingForInjectionPointSelection();
    m_pGraphicsViewer->setSelectionMode(true);
    setOnlyOneVisible(layer);
    m_cuttedLayer = layer;
}

void VSimulationFacade::performCut()
{
    m_pLayersProcessor->cutLayer(m_pGraphicsViewer->getSelectedNodesIds(), m_cuttedLayer);
    updateConfiguration();
    emit cutPerformed();
}

uint VSimulationFacade::getCuttedLayer() const
{
    return m_cuttedLayer;
}

void VSimulationFacade::performTransformation()
{
    uint layerId = m_pGraphicsViewer->getTransformedLayerId();
    int layer = m_pLayersProcessor->getLayerNumberById(layerId);
    if (layer >= 0)
    {
        m_pLayersProcessor->transformateLayer(m_pGraphicsViewer->getTransformedNodesCoords(),
                                              static_cast<uint>(layer));
        m_pGraphicsViewer->updatePosition(layerId);
        emit translationPerformed();
    }
}

uint VSimulationFacade::getTranslatedLayerId() const
{
    return m_pGraphicsViewer->getTransformedLayerId();
}

void VSimulationFacade::setAllVisible()
{
    for (uint i=0; i < m_pLayersProcessor->getLayersNumber(); ++i)
        m_pLayersProcessor->setVisibleLayer(i, true);
    m_pGraphicsViewer->updateVisibility();
}

void VSimulationFacade::setOnlyOneVisible(uint layer)
{
    for (uint i=0; i < m_pLayersProcessor->getLayersNumber(); ++i)
    {
        if (i != layer)
            m_pLayersProcessor->setVisibleLayer(i, false);
        else
            m_pLayersProcessor->setVisibleLayer(i, true);
    }
    m_pGraphicsViewer->updateVisibility();
}

void VSimulationFacade::cancelCuttingLayer()
{
    m_pGraphicsViewer->enableSelection(false);
}


void VSimulationFacade::cancelDrag()
{
    if (m_pGraphicsViewer->isDragOn())
    {
        m_pGraphicsViewer->setDragMode(false);
        updateGraphicsPositions();
    }
}

void VSimulationFacade::showInjectionPoint()
{
    cancelCuttingLayer();
    cancelDrag();
    cancelWaitingForVacuumPointSelection();
    cancelWaitingForInjectionPointSelection();
    if (isSimulationStopped())
        applyInjectionAndVacuumPoints();
    m_pGraphicsViewer->showInjectionPoint();
}

void VSimulationFacade::showVacuumPoint()
{
    cancelCuttingLayer();
    cancelDrag();
    cancelWaitingForVacuumPointSelection();
    cancelWaitingForInjectionPointSelection();
    if (isSimulationStopped())
        applyInjectionAndVacuumPoints();
    m_pGraphicsViewer->showVacuumPoint();
}

VSimulationInfo VSimulationFacade::getSimulationInfo() const
{
    return m_pSimulator->getSimulationInfo();
}

void VSimulationFacade::loadSavedParameters()
{
    QSettings settings;
    VSimulationParameters param = m_pSimulator->getSimulationParameters();
    VResin resin = param.getResin();

    VResin newResin;
    double temperature, injectionPressure, vacuumPressure, q, r, s, timeLimit;
    float simInjectionDiameter, simVacuumDiameter;
    bool timeLimitMode, lifetimeConsidered, stopOnVacuumFull;

    newResin.name = settings.value(QStringLiteral("sim/resinName"), resin.name).toString();
    newResin.defaultViscosity = settings.value(QStringLiteral("sim/defaultViscosity"), resin.defaultViscosity).toDouble();
    newResin.viscTempcoef = settings.value(QStringLiteral("sim/viscTempcoef"), resin.viscTempcoef).toDouble();
    newResin.defaultLifetime = settings.value(QStringLiteral("sim/defaultLifetime"), resin.defaultLifetime).toDouble();
    newResin.lifetimeTempcoef = settings.value(QStringLiteral("sim/lifetimeTempcoef"), resin.lifetimeTempcoef).toDouble();

    temperature = settings.value(QStringLiteral("sim/temperature"), param.getTemperature()).toDouble();
    simInjectionDiameter = settings.value(QStringLiteral("sim/injectionDiameter"), param.getInjectionDiameter()).toFloat();
    injectionPressure = settings.value(QStringLiteral("sim/injectionPressure"), param.getInjectionPressure()).toDouble();

    simVacuumDiameter = settings.value(QStringLiteral("sim/vacuumDiameter"), param.getVacuumDiameter()).toFloat();
    vacuumPressure = settings.value(QStringLiteral("sim/vacuumPressure"), param.getVacuumPressure()).toDouble();

    q = settings.value(QStringLiteral("sim/coefQ"), param.getQ()).toDouble();
    r = settings.value(QStringLiteral("sim/coefR"), param.getR()).toDouble();
    s = settings.value(QStringLiteral("sim/coefS"), param.getS()).toDouble();

    timeLimit = settings.value(QStringLiteral("sim/timeLimit"), param.getTimeLimit()).toDouble();
    timeLimitMode = settings.value(QStringLiteral("sim/timeLimitMode"), m_pSimulator->isTimeLimitModeOn()).toBool();

    lifetimeConsidered = settings.value(QStringLiteral("sim/lifetimeConsidered"), m_pSimulator->isLifetimeConsidered()).toBool();
    stopOnVacuumFull = settings.value(QStringLiteral("sim/stopOnVacuumFull"), m_pSimulator->isVacuumFullLimited()).toBool();

    m_pSimulator->setResin(newResin);
    m_pSimulator->setTemperature(temperature);
    m_pSimulator->setInjectionDiameter(simInjectionDiameter);
    m_pSimulator->setInjectionPressure(injectionPressure);
    m_pSimulator->setVacuumDiameter(simVacuumDiameter);
    m_pSimulator->setVacuumPressure(vacuumPressure);
    m_pSimulator->setQ(q);
    m_pSimulator->setR(r);
    m_pSimulator->setS(s);    
    m_pSimulator->setTimeLimit(timeLimit);
    m_pSimulator->setTimeLimitMode(timeLimitMode);
    m_pSimulator->considerLifetime(lifetimeConsidered);
    m_pSimulator->stopOnVacuumFull(stopOnVacuumFull);

    bool isOrthographic;
    float cubeSide;
    bool additionalControlsEnabled;
    isOrthographic = settings.value(QStringLiteral("graphics/cameraType"), m_pGraphicsViewer->isCameraOrthographic()).toBool();
    cubeSide = settings.value(QStringLiteral("graphics/cubeSide"), m_pGraphicsViewer->getCubeSide()).toFloat();
    additionalControlsEnabled = settings.value(QStringLiteral("graphics/additionalControls"),
                                               m_pGraphicsViewer->areAdditionalControlsEnabled()).toBool();
    m_pGraphicsViewer->setCameraOrthographic(isOrthographic);
    m_pGraphicsViewer->setCubeSide(cubeSide);
    m_pGraphicsViewer->enableAdditionalControls(additionalControlsEnabled);

    bool useTableParam;
    QVector2D tableSize, tableInjectionCoords, tableVacuumCoords, injectionCoords, vacuumCoords;
    float tableInjectionDiameter, tableVacuumDiameter, injectionDiameter, vacuumDiameter;

    tableSize = settings.value(QStringLiteral("table/size"), m_pTable->getSize()).value<QVector2D>();
    tableInjectionDiameter = settings.value(QStringLiteral("table/injectionDiameter"), m_pTable->getInjectionDiameter()).toFloat();
    tableVacuumDiameter = settings.value(QStringLiteral("table/vacuumDiameter"), m_pTable->getVacuumDiameter()).toFloat();
    tableInjectionCoords = settings.value(QStringLiteral("table/injectionCoords"), m_pTable->getInjectionCoords()).value<QVector2D>();
    tableVacuumCoords = settings.value(QStringLiteral("table/vacuumCoords"), m_pTable->getVacuumCoords()).value<QVector2D>();

    useTableParam = settings.value(QStringLiteral("injectionVacuum/useTableParam"), m_useTableParameters).toBool();
    injectionDiameter = settings.value(QStringLiteral("injectionVacuum/injectionDiameter"), m_pInjectionVacuum->injectionDiameter).toFloat();
    vacuumDiameter = settings.value(QStringLiteral("injectionVacuum/vacuumDiameter"), m_pInjectionVacuum->vacuumDiameter).toFloat();
    injectionCoords = settings.value(QStringLiteral("injectionVacuum/injectionCoords"), m_pInjectionVacuum->injectionCoords).value<QVector2D>();
    vacuumCoords = settings.value(QStringLiteral("injectionVacuum/vacuumCoords"), m_pInjectionVacuum->vacuumCoords).value<QVector2D>();

    setTableSize(tableSize.x(), tableSize.y());
    setTableInjectionDiameter(tableInjectionDiameter);
    setTableVacuumDiameter(tableVacuumDiameter);
    setTableInjectionCoords(tableInjectionCoords.x(), tableInjectionCoords.y());
    setTableVacuumCoords(tableVacuumCoords.x(), tableVacuumCoords.y());

    useTableParameters(useTableParam);
    m_pInjectionVacuum->injectionDiameter = injectionDiameter;
    m_pInjectionVacuum->vacuumDiameter = vacuumDiameter;
    m_pInjectionVacuum->injectionCoords = injectionCoords;
    m_pInjectionVacuum->vacuumCoords = vacuumCoords;
}

void VSimulationFacade::saveParameters() const
{
    QSettings settings;
    VSimulationParameters param = m_pSimulator->getSimulationParameters();
    VResin resin = param.getResin();
    settings.setValue(QStringLiteral("sim/resinName"), resin.name);
    settings.setValue(QStringLiteral("sim/defaultViscosity"), resin.defaultViscosity);
    settings.setValue(QStringLiteral("sim/viscTempcoef"), resin.viscTempcoef);
    settings.setValue(QStringLiteral("sim/defaultLifetime"), resin.defaultLifetime);
    settings.setValue(QStringLiteral("sim/lifetimeTempcoef"), resin.lifetimeTempcoef);

    settings.setValue(QStringLiteral("sim/temperature"), param.getTemperature());
    settings.setValue(QStringLiteral("sim/injectionDiameter"), param.getInjectionDiameter());
    settings.setValue(QStringLiteral("sim/injectionPressure"), param.getInjectionPressure());

    settings.setValue(QStringLiteral("sim/vacuumDiameter"), param.getVacuumDiameter());
    settings.setValue(QStringLiteral("sim/vacuumPressure"), param.getVacuumPressure());

    settings.setValue(QStringLiteral("sim/coefQ"), param.getQ());
    settings.setValue(QStringLiteral("sim/coefR"), param.getR());
    settings.setValue(QStringLiteral("sim/coefS"), param.getS());

    settings.setValue(QStringLiteral("sim/timeLimit"), param.getTimeLimit());
    settings.setValue(QStringLiteral("sim/timeLimitMode"), m_pSimulator->isTimeLimitModeOn());
    settings.setValue(QStringLiteral("sim/lifetimeConsidered"), m_pSimulator->isLifetimeConsidered());
    settings.setValue(QStringLiteral("sim/stopOnVacuumFull"), m_pSimulator->isVacuumFullLimited());

    settings.setValue(QStringLiteral("graphics/cameraType"), m_pGraphicsViewer->isCameraOrthographic());
    settings.setValue(QStringLiteral("graphics/cubeSide"), m_pGraphicsViewer->getCubeSide());
    settings.setValue(QStringLiteral("graphics/additionalControls"), m_pGraphicsViewer->areAdditionalControlsEnabled());

    settings.setValue(QStringLiteral("table/size"), m_pTable->getSize());
    settings.setValue(QStringLiteral("table/injectionDiameter"), m_pTable->getInjectionDiameter());
    settings.setValue(QStringLiteral("table/vacuumDiameter"), m_pTable->getVacuumDiameter());
    settings.setValue(QStringLiteral("table/injectionCoords"), m_pTable->getInjectionCoords());
    settings.setValue(QStringLiteral("table/vacuumCoords"), m_pTable->getVacuumCoords());

    settings.setValue(QStringLiteral("injectionVacuum/useTableParam"), m_useTableParameters);
    settings.setValue(QStringLiteral("injectionVacuum/injectionDiameter"), m_pInjectionVacuum->injectionDiameter);
    settings.setValue(QStringLiteral("injectionVacuum/vacuumDiameter"), m_pInjectionVacuum->vacuumDiameter);
    settings.setValue(QStringLiteral("injectionVacuum/injectionCoords"), m_pInjectionVacuum->injectionCoords);
    settings.setValue(QStringLiteral("injectionVacuum/vacuumCoords"), m_pInjectionVacuum->vacuumCoords);

    settings.sync();
}

void VSimulationFacade::enableInteraction()
{
    if (m_pGraphicsViewer)
    {
        m_pGraphicsViewer->enableDrag(true);
    }
}

void VSimulationFacade::setCubeSide(float side)
{
    m_pGraphicsViewer->setCubeSide(side);
}

float VSimulationFacade::getCubeSide() const
{
    return m_pGraphicsViewer->getCubeSide();
}

const QWidget * VSimulationFacade::getGLWidget() const
{
    return m_pGraphicsViewer->getGLWidget();
}

void VSimulationFacade::disableInteraction()
{
    if (m_pGraphicsViewer)
    {
        cancelCuttingLayer();
        cancelDrag();
        cancelWaitingForVacuumPointSelection();
        cancelWaitingForInjectionPointSelection();
        m_pGraphicsViewer->enableDrag(false);
    }
}

bool VSimulationFacade::isSimulationActive() const
{
    return m_pSimulator->isSimulating();
}

bool VSimulationFacade::isSimulationStopped() const
{
    return !(m_pSimulator->isSimulating() || m_pSimulator->isPaused());
}

bool VSimulationFacade::isSimulationPaused() const
{
    return m_pSimulator->isPaused();
}

std::shared_ptr<const VTable> VSimulationFacade::getTable() const
{
    return m_pTable;
}

void VSimulationFacade::setTableSize(float width, float height)
{
    m_pTable->setSize(width, height);
    const QVector2D &size = m_pTable->getSize();
    emit tableSizeSet(size.x(), size.y());
}

void VSimulationFacade::setTableInjectionCoords(float x, float y)
{
    m_pTable->setInjectionCoords(x, y);
    const QVector2D &coords = m_pTable->getInjectionCoords();
    emit tableInjectionCoordsSet(coords.x(), coords.y());
    if (m_useTableParameters)
    {
        m_pInjectionVacuum->injectionCoords = coords;
    }
}

void VSimulationFacade::setTableVacuumCoords(float x, float y)
{
    m_pTable->setVacuumCoords(x, y);
    const QVector2D &coords = m_pTable->getVacuumCoords();
    emit tableVacuumCoordsSet(coords.x(), coords.y());
    if (m_useTableParameters)
    {
        m_pInjectionVacuum->vacuumCoords = coords;
    }
}

void VSimulationFacade::setTableInjectionDiameter(float diameter)
{
    m_pTable->setInjectionDiameter(diameter);
    emit tableInjectionDiameterSet(m_pTable->getInjectionDiameter());
    if (m_useTableParameters)
    {
        m_pInjectionVacuum->injectionDiameter = m_pTable->getInjectionDiameter();
        m_pSimulator->setInjectionDiameter(m_pTable->getInjectionDiameter());
    }
}

void VSimulationFacade::setTableVacuumDiameter(float diameter)
{
    m_pTable->setVacuumDiameter(diameter);
    emit tableVacuumDiameterSet(m_pTable->getVacuumDiameter());
    if (m_useTableParameters)
    {
        m_pInjectionVacuum->vacuumDiameter = m_pTable->getVacuumDiameter();
        m_pSimulator->setVacuumDiameter(m_pTable->getVacuumDiameter());
    }
}

void VSimulationFacade::applyInjectionAndVacuumPoints()
{
    const VInjectionVacuum * injectionVacuum;
    if (m_useTableParameters)
    {
        injectionVacuum = &(m_pTable->getInjectionVacuum());
    }
    else
    {
        injectionVacuum = m_pInjectionVacuum.get();
    }
    m_pSimulator->setInjectionDiameter(injectionVacuum->injectionDiameter);
    m_pSimulator->setVacuumDiameter(injectionVacuum->injectionDiameter);
    m_pLayersProcessor->setInjectionPoint(injectionVacuum->injectionCoords,
                                          injectionVacuum->injectionDiameter);
    emit injectionPointSet();
    m_pLayersProcessor->setVacuumPoint(injectionVacuum->vacuumCoords,
                                       injectionVacuum->vacuumDiameter);
    emit vacuumPointSet();
}

void VSimulationFacade::useTableParameters(bool use)
{
    m_useTableParameters = use;
    if (use)
    {
        *m_pInjectionVacuum = m_pTable->getInjectionVacuum();
        m_pSimulator->setInjectionDiameter(m_pTable->getInjectionDiameter());
        m_pSimulator->setVacuumDiameter(m_pTable->getVacuumDiameter());
    }
    emit useTableParametersSet(use);
}

bool VSimulationFacade::isUsingTableParameters() const
{
    return m_useTableParameters;
}

void VSimulationFacade::setTable(const std::shared_ptr<VTable> &p_table)
{
    m_pTable = p_table;
    emit tableSizeSet(m_pTable->getSize().x(), m_pTable->getSize().y());
    emit tableInjectionCoordsSet(m_pTable->getInjectionCoords().x(), m_pTable->getInjectionCoords().y());
    emit tableVacuumCoordsSet(m_pTable->getVacuumCoords().x(), m_pTable->getVacuumCoords().y());
    emit tableInjectionDiameterSet(m_pTable->getInjectionDiameter());
    emit tableVacuumDiameterSet(m_pTable->getVacuumDiameter());
    if (m_useTableParameters)
    {
        m_pSimulator->setInjectionDiameter(m_pTable->getInjectionDiameter());
        m_pSimulator->setVacuumDiameter(m_pTable->getVacuumDiameter());
        m_pInjectionVacuum->injectionDiameter = m_pTable->getInjectionDiameter();
        m_pInjectionVacuum->vacuumDiameter = m_pTable->getVacuumDiameter();
    }
}

std::vector<std::vector<QPolygonF> > VSimulationFacade::getAllActivePolygons() const
{
    return m_pLayersProcessor->getAllActivePolygons();
}

const QString & VSimulationFacade::getLayerName(uint layer) const
{
    return m_pLayersProcessor->getLayerName(layer);
}

void VSimulationFacade::setLayerName(uint layer, const QString &name)
{
    m_pLayersProcessor->setLayerName(layer, name);
    emit configUpdated();
}

void VSimulationFacade::m_on_got_point(const QVector3D &point)
{
    QVector2D xyProjection = point.toVector2D();
    if(m_selectInjectionPoint)
    {
        m_pInjectionVacuum->injectionCoords = xyProjection;
        m_pLayersProcessor->setInjectionPoint(xyProjection, m_pInjectionVacuum->injectionDiameter);
        m_pSimulator->setInjectionDiameter(m_pInjectionVacuum->injectionDiameter);
        m_selectInjectionPoint = false;
        m_pGraphicsViewer->showInjectionPoint();
        emit injectionPointSet();
    }
    else if (m_selectVacuumPoint)
    {
        m_pInjectionVacuum->vacuumCoords = xyProjection;
        m_pLayersProcessor->setVacuumPoint(xyProjection, m_pInjectionVacuum->vacuumDiameter);
        m_pSimulator->setVacuumDiameter(m_pInjectionVacuum->vacuumDiameter);
        m_selectVacuumPoint = false;
        m_pGraphicsViewer->showVacuumPoint();
        emit vacuumPointSet();
    }
}

void VSimulationFacade::m_on_got_nodes_selection(const VGraphicsViewer::const_uint_vect_ptr &pSelectedNodesIds)
{
    #ifdef DEBUG_MODE
        qInfo() << "Selected Ids:";
        for (auto id : *pSelectedNodesIds)
            qInfo() << id;
    #endif
    emit selectionMade();
}
