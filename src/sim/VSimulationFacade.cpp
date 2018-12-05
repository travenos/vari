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
    m_pLayersProcessor(new VLayersProcessor)
{
    SoQt::init(parent);
    m_pGraphicsViewer.reset(new VGraphicsViewer(parent, m_pSimulator));
    std::shared_ptr<std::mutex> p_nodesLock(new std::mutex);
    std::shared_ptr<std::mutex> p_trianglesLock(new std::mutex);
    m_pSimulator->setMutexes(p_nodesLock, p_trianglesLock);
    m_pGraphicsViewer->setMutexes(p_nodesLock, p_trianglesLock);
    m_pLayersProcessor->setMutexes(p_nodesLock, p_trianglesLock);
}

void VSimulationFacade::startSimulation() noexcept
{
    m_pSimulator->start();
}

void VSimulationFacade::stopSimulation() noexcept
{
    m_pSimulator->stop();
}

void VSimulationFacade::resetSimulation() noexcept
{
    m_pSimulator->reset();
}

/**
 * @return int
 */
int VSimulationFacade::getLayersNumber() const noexcept
{
    return m_pLayersProcessor->getLayersNumber();
}

/**
 * @return int
 */
int VSimulationFacade::getActiveLayersNumber() const noexcept
{
    return m_pLayersProcessor->getActiveLayersNumber();
}

/**
 * @return int
 */
int VSimulationFacade::getInactiveLayersNumber() const noexcept
{
    return m_pLayersProcessor->getInactiveLayersNumber();
}

/**
 * @param layer
 * @param visible
 */
void VSimulationFacade::setVisible(unsigned int layer, bool visible) noexcept(false)
{
    m_pLayersProcessor->setVisibleLayer(layer, visible);
    VSimNode::const_vector_ptr nodes = m_pLayersProcessor->getActiveNodes();
    VSimTriangle::const_vector_ptr triangles = m_pLayersProcessor->getActiveTriangles();
    m_pGraphicsViewer->setGraphicsElements(nodes, triangles);
}

/**
 * @param layer
 */
void VSimulationFacade::removeLayer(unsigned int layer) noexcept(false)
{
    m_pLayersProcessor->removeLayer(layer);
    updateConfiguration();
}

/**
 * @param layer
 */
void VSimulationFacade::enableLayer(unsigned int layer, bool enable) noexcept(false)
{
    m_pLayersProcessor->enableLayer(layer, enable);
    updateConfiguration();
}

/**
 * @param layer
 * @param material
 */
void VSimulationFacade::setMaterial(unsigned int layer, const VCloth &material) noexcept(false)
{
    m_pLayersProcessor->setMaterial(layer, material);
}

void VSimulationFacade::draw() noexcept
{
    //TODO remove this method
    m_pGraphicsViewer->render();
}

/**
 * @param pressure
 */
void VSimulationFacade::setInjectionPressure(double pressure) noexcept
{
    m_pSimulator->setInjectionPressure(pressure);
}

void VSimulationFacade::setVacuumPressure(double pressure) noexcept
{
    m_pSimulator->setVacuumPressure(pressure);
}

/**
 * @param diameter
 */
void VSimulationFacade::setInjectionDiameter(double diameter) noexcept
{
    m_pSimulator->setInjectionDiameter(diameter);
}

/**
 * @param diameter
 */
void VSimulationFacade::setVacuumDiameter(double diameter) noexcept
{
    m_pSimulator->setVacuumDiameter(diameter);
}

void VSimulationFacade::setDefaultViscosity(double defaultViscosity) noexcept
{
    m_pSimulator->setDefaultViscosity(defaultViscosity);
}


void VSimulationFacade::setTemperature(double temperature) noexcept
{
    m_pSimulator->setTemperature(temperature);
}


void VSimulationFacade::setTempcoef(double tempcoef) noexcept
{
    m_pSimulator->setTempcoef(tempcoef);
}

void VSimulationFacade::setQ(double q) noexcept
{
    m_pSimulator->setQ(q);
}
void VSimulationFacade::setR(double r) noexcept
{
    m_pSimulator->setR(r);
}
void VSimulationFacade::setS(double s) noexcept
{
    m_pSimulator->setS(s);
}

void VSimulationFacade::newModel() noexcept
{
    m_pSimulator->clear();
    m_pLayersProcessor->clear();
    m_pGraphicsViewer->clearAll();
}

VCloth::const_ptr VSimulationFacade::getMaterial(unsigned int layer) const noexcept(false)
{
    return m_pLayersProcessor->getMaterial(layer);
}

VSimulationParametres::const_ptr VSimulationFacade::getParametres() const noexcept
{
    return m_pSimulator->getSimulationParametres();
}

bool VSimulationFacade::isLayerVisible(unsigned int layer) const noexcept(false)
{
    return m_pLayersProcessor->isLayerVisible(layer);
}

bool VSimulationFacade::isLayerEnabled(unsigned int layer) const noexcept(false)
{
    return m_pLayersProcessor->isLayerEnabled(layer);
}

/**
 * @param filename
 */
void VSimulationFacade::newLayerFromFile(const VCloth &material, const QString &filename) noexcept(false)
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
                                                            m_pSimulator->getSimulationParametres());
                break;
            }
            else if(line.contains("ANSYS",Qt::CaseInsensitive))
            {
                p_layerBuilder = new VLayerFromAnsysBuilder(filename,
                                                            material,
                                                            m_pSimulator->getSimulationParametres());
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
            m_pGraphicsViewer->viewAll();
            m_pGraphicsViewer->saveHomePosition();
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

void VSimulationFacade::updateConfiguration() noexcept(false)
{
    VSimNode::const_vector_ptr nodes = m_pLayersProcessor->getActiveNodes();
    VSimTriangle::const_vector_ptr triangles = m_pLayersProcessor->getActiveTriangles();
    m_pSimulator->setActiveElements(nodes, triangles);
    m_pGraphicsViewer->setGraphicsElements(nodes, triangles);
}
