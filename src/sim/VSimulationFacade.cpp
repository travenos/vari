/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include <Inventor/Qt/SoQt.h>
#include <QFile>
#include <QSettings>

#include "VSimulationFacade.h"
#include "VLayerFromGmeshBuilder.h"
#include "VLayerFromAnsysBuilder.h"
#include "VExceptions.h"

/**
 * VSimulationFacade implementation
 */


VSimulationFacade::VSimulationFacade(QWidget *parent):
    m_pSimulator(new VSimulator),
    m_pLayersProcessor(new VLayersProcessor),
    m_selectInjectionPoint(false),
    m_selectVacuumPoint(false)
{
    SoQt::init(parent);
    m_pGraphicsViewer.reset(new VGraphicsViewer(parent, m_pSimulator));
    std::shared_ptr<std::mutex> p_nodesLock(new std::mutex);
    std::shared_ptr<std::mutex> p_trianglesLock(new std::mutex);
    m_pSimulator->setMutexes(p_nodesLock, p_trianglesLock);
    m_pGraphicsViewer->setMutexes(p_nodesLock, p_trianglesLock);
    m_pLayersProcessor->setMutexes(p_nodesLock, p_trianglesLock);
    connectSignals();
}

void VSimulationFacade::connectSignals()
{
    connect(m_pGraphicsViewer.get(), SIGNAL(gotPoint(const QVector3D &)),
            this, SLOT(m_on_got_point(const QVector3D &)));
    connect(m_pSimulator.get(), SIGNAL(simulationStarted()),
            this, SIGNAL(simulationStarted()));
    connect(m_pSimulator.get(), SIGNAL(simulationPaused()),
            this, SIGNAL(simulationPaused()));
    connect(m_pSimulator.get(), SIGNAL(simulationStopped()),
            this, SIGNAL(simulationStopped()));
    connect(m_pGraphicsViewer.get(), SIGNAL(askForDisplayingInfo()),
            this, SIGNAL(gotNewInfo()));

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
}

void VSimulationFacade::startSimulation() 
{
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

/**
 * @return int
 */
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

/**
 * @param layer
 * @param visible
 */
void VSimulationFacade::setVisible(unsigned int layer, bool visible) 
{
    m_pLayersProcessor->setVisibleLayer(layer, visible);
    m_pGraphicsViewer->updateVisibility();
    emit layerVisibilityChanged(layer, visible);
}

/**
 * @param layer
 */
void VSimulationFacade::removeLayer(unsigned int layer) 
{
    m_pLayersProcessor->removeLayer(layer);
    updateConfiguration();
    emit layerRemoved(layer);
}

/**
 * @param layer
 */
void VSimulationFacade::enableLayer(unsigned int layer, bool enable) 
{
    m_pLayersProcessor->enableLayer(layer, enable);
    updateConfiguration();
    emit layerEnabled(layer, enable);
}

/**
 * @param layer
 * @param material
 */
void VSimulationFacade::setMaterial(unsigned int layer, const VCloth &material) 
{
    m_pLayersProcessor->setMaterial(layer, material);
    m_pSimulator->resetTriangleColors();
    m_pGraphicsViewer->updateColors();
    emit materialChanged(layer);
}

void VSimulationFacade::setResin(const VResin& resin)
{
    m_pSimulator->setResin(resin);
}

/**
 * @param pressure
 */
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

void VSimulationFacade::newModel() 
{
    m_pSimulator->clear();
    m_pLayersProcessor->clear();
    m_pGraphicsViewer->clearAll();
}

VCloth::const_ptr VSimulationFacade::getMaterial(unsigned int layer) const 
{
    return m_pLayersProcessor->getMaterial(layer);
}

VSimulationParametres::const_ptr VSimulationFacade::getParametres() const 
{
    return m_pSimulator->getSimulationParametres();
}

bool VSimulationFacade::isLayerVisible(unsigned int layer) const 
{
    return m_pLayersProcessor->isLayerVisible(layer);
}

bool VSimulationFacade::isLayerEnabled(unsigned int layer) const 
{
    return m_pLayersProcessor->isLayerEnabled(layer);
}

/**
 * @param filename
 */
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
                                                            m_pSimulator->getSimulationParametres(),
                                                            units);
                break;
            }
            else if(line.contains("ANSYS",Qt::CaseInsensitive))
            {
                p_layerBuilder = new VLayerFromAnsysBuilder(filename,
                                                            material,
                                                            m_pSimulator->getSimulationParametres(),
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
            emit layerAdded();
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
    VSimNode::const_vector_ptr nodes = m_pLayersProcessor->getActiveNodes();
    VSimTriangle::const_vector_ptr triangles = m_pLayersProcessor->getActiveTriangles();
    m_pSimulator->setActiveElements(nodes, triangles);
    m_pGraphicsViewer->setGraphicsElements(nodes, triangles);
}

void VSimulationFacade::waitForInjectionPointSelection(double diameter)
{
    m_selectInjectionPoint = true;
    m_selectVacuumPoint = false;
    m_injectionDiameter = diameter;
    m_pGraphicsViewer->viewFromAbove();
    m_pGraphicsViewer->setViewing(false);
    //TODO forbid rotation
    emit canceledWaitingForVacuumPoint();
    emit startedWaitingForInjectionPoint();
}

void VSimulationFacade::waitForVacuumPointSelection(double diameter)
{
    m_selectVacuumPoint = true;
    m_selectInjectionPoint = false;
    m_vacuumDiameter = diameter;
    m_pGraphicsViewer->viewFromAbove();
    m_pGraphicsViewer->setViewing(false);
    //TODO forbid rotation
    emit canceledWaitingForInjectionPoint();
    emit startedWaitingForVacuumPoint();
}

void VSimulationFacade::cancelWaitingForInjectionPointSelection()
{
    m_selectInjectionPoint = false;
    //TODO allow rotation
    emit canceledWaitingForInjectionPoint();
}

void VSimulationFacade::cancelWaitingForVacuumPointSelection()
{
    m_selectVacuumPoint = false;
    //TODO allow rotation
    emit canceledWaitingForVacuumPoint();
}

void VSimulationFacade::showInjectionPoint()
{
    m_pGraphicsViewer->showInjectionPoint();
}

void VSimulationFacade::showVacuumPoint()
{
    m_pGraphicsViewer->showVacuumPoint();
}

VSimulationInfo VSimulationFacade::getSimulationInfo() const
{
    return m_pSimulator->getSimulationInfo();
}

void VSimulationFacade::loadSavedParametres()
{
    QSettings settings;
    VSimulationParametres::const_ptr param = m_pSimulator->getSimulationParametres();
    VResin resin = param->getResin();

    VResin newResin;
    double temperature, injectionDiameter, injectionPressure, vacuumDiameter, vacuumPressure, q, r, s;

    newResin.name = settings.value(QStringLiteral("sim/resinName"), resin.name).toString();
    newResin.defaultViscosity = settings.value(QStringLiteral("sim/defaultViscosity"), resin.defaultViscosity).toDouble();
    newResin.tempcoef = settings.value(QStringLiteral("sim/tempcoef"), resin.tempcoef).toDouble();

    temperature = settings.value(QStringLiteral("sim/temperature"), param->getTemperature()).toDouble();
    injectionDiameter = settings.value(QStringLiteral("sim/injectionDiameter"), param->getInjectionDiameter()).toDouble();
    injectionPressure = settings.value(QStringLiteral("sim/injectionPressure"), param->getInjectionPressure()).toDouble();

    vacuumDiameter = settings.value(QStringLiteral("sim/vacuumDiameter"), param->getVacuumDiameter()).toDouble();
    vacuumPressure = settings.value(QStringLiteral("sim/vacuumPressure"), param->getVacuumPressure()).toDouble();

    q = settings.value(QStringLiteral("sim/coefQ"), param->getQ()).toDouble();
    r = settings.value(QStringLiteral("sim/coefR"), param->getR()).toDouble();
    s = settings.value(QStringLiteral("sim/coefS"), param->getS()).toDouble();

    m_pSimulator->setResin(newResin);
    m_pSimulator->setTemperature(temperature);
    m_pSimulator->setInjectionDiameter(injectionDiameter);
    m_pSimulator->setInjectionPressure(injectionPressure);
    m_pSimulator->setVacuumDiameter(vacuumDiameter);
    m_pSimulator->setVacuumPressure(vacuumPressure);
    m_pSimulator->setQ(q);
    m_pSimulator->setR(r);
    m_pSimulator->setS(s);
}

void VSimulationFacade::saveParametres() const
{
    QSettings settings;
    VSimulationParametres::const_ptr param = m_pSimulator->getSimulationParametres();
    VResin resin = param->getResin();
    settings.setValue(QStringLiteral("sim/resinName"), resin.name);
    settings.setValue(QStringLiteral("sim/defaultViscosity"), resin.defaultViscosity);
    settings.setValue(QStringLiteral("sim/tempcoef"), resin.tempcoef);

    settings.setValue(QStringLiteral("sim/temperature"), param->getTemperature());
    settings.setValue(QStringLiteral("sim/injectionDiameter"), param->getInjectionDiameter());
    settings.setValue(QStringLiteral("sim/injectionPressure"), param->getInjectionPressure());

    settings.setValue(QStringLiteral("sim/vacuumDiameter"), param->getVacuumDiameter());
    settings.setValue(QStringLiteral("sim/vacuumPressure"), param->getVacuumPressure());

    settings.setValue(QStringLiteral("sim/coefQ"), param->getQ());
    settings.setValue(QStringLiteral("sim/coefR"), param->getR());
    settings.setValue(QStringLiteral("sim/coefS"), param->getS());

    settings.sync();
}

void VSimulationFacade::m_on_got_point(const QVector3D &point)
{
    if(m_selectInjectionPoint)
    {
        m_pLayersProcessor->setInjectionPoint(point, m_injectionDiameter);
        m_pSimulator->setInjectionDiameter(m_injectionDiameter);
        m_selectInjectionPoint = false;
        //TODO allow rotation
        emit injectionPointSet();
    }
    else if (m_selectVacuumPoint)
    {
        m_pLayersProcessor->setVacuumPoint(point, m_vacuumDiameter);
        m_pSimulator->setVacuumDiameter(m_vacuumDiameter);
        m_selectVacuumPoint = false;
        //TODO allow rotation
        emit vacuumPointSet();
    }
}
