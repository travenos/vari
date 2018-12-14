/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include <Inventor/Qt/SoQt.h>
#include <QFile>

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
    emit materialChanged(layer);
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

void VSimulationFacade::setDefaultViscosity(double defaultViscosity) 
{
    m_pSimulator->setDefaultViscosity(defaultViscosity);
}


void VSimulationFacade::setTemperature(double temperature) 
{
    m_pSimulator->setTemperature(temperature);
}


void VSimulationFacade::setTempcoef(double tempcoef) 
{
    m_pSimulator->setTempcoef(tempcoef);
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
}

void VSimulationFacade::waitForVacuumPointSelection(double diameter)
{
    m_selectVacuumPoint = true;
    m_selectInjectionPoint = false;
    m_vacuumDiameter = diameter;
    m_pGraphicsViewer->viewFromAbove();
    m_pGraphicsViewer->setViewing(false);
    //TODO forbid rotation
}

void VSimulationFacade::cancelWaitingForInjectionPointSelection()
{
    m_selectInjectionPoint = false;
    //TODO allow rotation
}

void VSimulationFacade::cancelWaitingForVacuumPointSelection()
{
    m_selectVacuumPoint = false;
    //TODO allow rotation
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
