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
#include "import_export/VModelExport.h"
#include "import_export/VModelImport.h"
#include "structures/VExceptions.h"

/**
 * VSimulationFacade implementation
 */


VSimulationFacade::VSimulationFacade(QWidget *parent):
    m_parentWidget(parent),
    m_pSimulator(new VSimulator),
    m_pLayersProcessor(new VLayersProcessor),
    m_selectInjectionPoint(false),
    m_selectVacuumPoint(false),
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
    connect(m_pGraphicsViewer.get(), SIGNAL(askForDisplayingInfo()),
            this, SIGNAL(gotNewInfo()));
    connect(m_pGraphicsViewer.get(), SIGNAL(selectionEnabled(bool)),
            this, SIGNAL(selectionEnabled(bool)));
    connect(m_pSimulator.get(), SIGNAL(simulationStarted()),
            this, SIGNAL(simulationStarted()));
    connect(m_pSimulator.get(), SIGNAL(simulationPaused()),
            this, SIGNAL(simulationPaused()));
    connect(m_pSimulator.get(), SIGNAL(simulationStopped()),
            this, SIGNAL(simulationStopped()));

    connect(m_pSimulator.get(), SIGNAL(resinChanged()),
            this, SIGNAL(resinChanged()));

    connect(m_pSimulator.get(), SIGNAL(injectionDiameterSet(double)),
            this, SIGNAL(injectionDiameterSet(double)));
    connect(m_pSimulator.get(), SIGNAL(vacuumDiameterSet(double)),
            this, SIGNAL(vacuumDiameterSet(double)));
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
    connect(m_pLayersProcessor.get(), SIGNAL(layerAdded()),
            this, SIGNAL(layerAdded()));
    connect(m_pLayersProcessor.get(), SIGNAL(layersCleared()),
            this, SIGNAL(layersCleared()));
}

void VSimulationFacade::startSimulation() 
{
    if (!m_pLayersProcessor->areLayersConnected())
        m_pLayersProcessor->createConnections();
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
    m_pGraphicsViewer->updateVisibility();
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

void VSimulationFacade::setMaterial(uint layer, const VCloth &material)
{
    m_pLayersProcessor->setMaterial(layer, material);
    m_pSimulator->resetTriangleColors();
    m_pGraphicsViewer->updateColors();
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

void VSimulationFacade::newModel() 
{
    m_pSimulator->clear();
    m_pLayersProcessor->clear();
    m_pGraphicsViewer->clearAll();
}

void VSimulationFacade::loadModel(const QString &filename)
{
    m_pSimulator->pause();
    VModelImport loader;
    loader.loadFromFile(filename);
    m_pLayersProcessor = loader.getLayersProcessor();
    initLayersProcessor();
    updateConfiguration();
    m_pSimulator->setSimulationParametres(loader.getInfo(), loader.getSimulationParametres(),
                                          loader.getPaused(), loader.getTimeLimited());
    m_pGraphicsViewer->viewFromAbove();
    emit modelLoaded();
}

void VSimulationFacade::saveModel(const QString &filename)
{
    m_pSimulator->pause();
    VSimulationInfo info = m_pSimulator->getSimulationInfo();
    VSimulationParametres param = m_pSimulator->getSimulationParametres();
    bool paused = m_pSimulator->isPaused();
    bool timeLimited = m_pSimulator->isTimeLimitModeOn();
    VModelExport saver(info, param, m_pLayersProcessor, paused, timeLimited);
    saver.saveToFile(filename);
    emit modelSaved();
}

VCloth::const_ptr VSimulationFacade::getMaterial(uint layer) const
{
    return m_pLayersProcessor->getMaterial(layer);
}

VSimulationParametres VSimulationFacade::getParametres() const
{
    return m_pSimulator->getSimulationParametres();
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

void VSimulationFacade::updateConfiguration() 
{
    m_pSimulator->setActiveElements(m_pLayersProcessor->getActiveNodes(),
                                    m_pLayersProcessor->getActiveTriangles());
    m_pGraphicsViewer->setGraphicsElements(m_pLayersProcessor->getLayers());
    emit configUpdated();
}

void VSimulationFacade::waitForInjectionPointSelection(double diameter)
{
    cancelCuttingLayer();
    cancelDrag();
    cancelWaitingForVacuumPointSelection();
    m_pGraphicsViewer->enableDrag(false);
    m_selectInjectionPoint = true;
    m_selectVacuumPoint = false;
    m_injectionDiameter = diameter;
    m_pGraphicsViewer->viewFromAbove();
    m_pGraphicsViewer->setViewing(false);
    emit startedWaitingForInjectionPoint();
}

void VSimulationFacade::waitForVacuumPointSelection(double diameter)
{
    cancelCuttingLayer();
    cancelDrag();
    cancelWaitingForInjectionPointSelection();
    m_pGraphicsViewer->enableDrag(false);
    m_selectVacuumPoint = true;
    m_vacuumDiameter = diameter;
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
    m_pLayersProcessor->transformateLayer(m_pGraphicsViewer->getTransformedNodesCoords(),
                                 m_pGraphicsViewer->getTransformedLayerNumber());
    updateConfiguration();
    emit translationPerformed();
}

uint VSimulationFacade::getTranslatedLayer() const
{
    return m_pGraphicsViewer->getTransformedLayerNumber();
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
        m_pGraphicsViewer->setGraphicsElements(m_pLayersProcessor->getLayers());
    }
}

void VSimulationFacade::showInjectionPoint()
{
    cancelCuttingLayer();
    cancelDrag();
    cancelWaitingForVacuumPointSelection();
    cancelWaitingForInjectionPointSelection();
    m_pGraphicsViewer->showInjectionPoint();
}

void VSimulationFacade::showVacuumPoint()
{
    cancelCuttingLayer();
    cancelDrag();
    cancelWaitingForVacuumPointSelection();
    cancelWaitingForInjectionPointSelection();
    m_pGraphicsViewer->showVacuumPoint();
}

VSimulationInfo VSimulationFacade::getSimulationInfo() const
{
    return m_pSimulator->getSimulationInfo();
}

void VSimulationFacade::loadSavedParametres()
{
    QSettings settings;
    VSimulationParametres param = m_pSimulator->getSimulationParametres();
    VResin resin = param.getResin();

    VResin newResin;
    double temperature, injectionDiameter, injectionPressure, vacuumDiameter, vacuumPressure, q, r, s, timeLimit;
    bool timeLimitMode;

    newResin.name = settings.value(QStringLiteral("sim/resinName"), resin.name).toString();
    newResin.defaultViscosity = settings.value(QStringLiteral("sim/defaultViscosity"), resin.defaultViscosity).toDouble();
    newResin.tempcoef = settings.value(QStringLiteral("sim/tempcoef"), resin.tempcoef).toDouble();

    temperature = settings.value(QStringLiteral("sim/temperature"), param.getTemperature()).toDouble();
    injectionDiameter = settings.value(QStringLiteral("sim/injectionDiameter"), param.getInjectionDiameter()).toDouble();
    injectionPressure = settings.value(QStringLiteral("sim/injectionPressure"), param.getInjectionPressure()).toDouble();

    vacuumDiameter = settings.value(QStringLiteral("sim/vacuumDiameter"), param.getVacuumDiameter()).toDouble();
    vacuumPressure = settings.value(QStringLiteral("sim/vacuumPressure"), param.getVacuumPressure()).toDouble();

    q = settings.value(QStringLiteral("sim/coefQ"), param.getQ()).toDouble();
    r = settings.value(QStringLiteral("sim/coefR"), param.getR()).toDouble();
    s = settings.value(QStringLiteral("sim/coefS"), param.getS()).toDouble();

    timeLimit = settings.value(QStringLiteral("sim/timeLimit"), param.getTimeLimit()).toDouble();
    timeLimitMode = settings.value(QStringLiteral("sim/timeLimitMode"), m_pSimulator->isTimeLimitModeOn()).toBool();

    m_pSimulator->setResin(newResin);
    m_pSimulator->setTemperature(temperature);
    m_pSimulator->setInjectionDiameter(injectionDiameter);
    m_pSimulator->setInjectionPressure(injectionPressure);
    m_pSimulator->setVacuumDiameter(vacuumDiameter);
    m_pSimulator->setVacuumPressure(vacuumPressure);
    m_pSimulator->setQ(q);
    m_pSimulator->setR(r);
    m_pSimulator->setS(s);    
    m_pSimulator->setTimeLimit(timeLimit);
    m_pSimulator->setTimeLimitMode(timeLimitMode);
}

void VSimulationFacade::saveParametres() const
{
    QSettings settings;
    VSimulationParametres param = m_pSimulator->getSimulationParametres();
    VResin resin = param.getResin();
    settings.setValue(QStringLiteral("sim/resinName"), resin.name);
    settings.setValue(QStringLiteral("sim/defaultViscosity"), resin.defaultViscosity);
    settings.setValue(QStringLiteral("sim/tempcoef"), resin.tempcoef);

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
    settings.sync();
}

void VSimulationFacade::enableInteraction()
{
    if (m_pGraphicsViewer)
    {
        m_pGraphicsViewer->enableDrag(true);
    }
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

void VSimulationFacade::m_on_got_point(const QVector3D &point)
{
    if(m_selectInjectionPoint)
    {
        m_pLayersProcessor->setInjectionPoint(point, m_injectionDiameter);
        m_pSimulator->setInjectionDiameter(m_injectionDiameter);
        m_selectInjectionPoint = false;
        emit injectionPointSet();
    }
    else if (m_selectVacuumPoint)
    {
        m_pLayersProcessor->setVacuumPoint(point, m_vacuumDiameter);
        m_pSimulator->setVacuumDiameter(m_vacuumDiameter);
        m_selectVacuumPoint = false;
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
