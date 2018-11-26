/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VSimulationFacade.h"
#include <Inventor/Qt/SoQt.h>

/**
 * VSimulationFacade implementation
 */


VSimulationFacade::VSimulationFacade(QWidget *parent):
    m_pSimulator(new VSimulator()),
    m_pLayersProcessor(new VLayersProcessor(m_pSimulator))
{
    SoQt::init(parent);
    m_pGraphicsViewer.reset(new VGraphicsViewer(parent, m_pSimulator));
    std::shared_ptr<std::mutex> p_nodesLock(new std::mutex);
    std::shared_ptr<std::mutex> p_trianglesLock(new std::mutex);
    m_pSimulator->setMutexes(p_nodesLock, p_trianglesLock);
    m_pGraphicsViewer->setMutexes(p_nodesLock, p_trianglesLock);
    m_pLayersProcessor->setMutexes(p_nodesLock, p_trianglesLock);
}

void VSimulationFacade::startSimulation() noexcept {

}

void VSimulationFacade::stopSimulation() noexcept {

}

void VSimulationFacade::resetSimulation() noexcept {

}

/**
 * @return int
 */
int VSimulationFacade::getLayersNumber() const noexcept {
    return 0;
}

/**
 * @return int
 */
int VSimulationFacade::getInactiveLayersNumber() const noexcept {
    return 0;
}

/**
 * @param layer
 * @param visible
 */
void VSimulationFacade::setVisible(unsigned int layer, bool visible) noexcept {

}

/**
 * @param layer
 */
void VSimulationFacade::removeLayer(unsigned int layer) noexcept {

}

/**
 * @param layer
 */
void VSimulationFacade::disableLayer(unsigned int layer) noexcept {

}

/**
 * @param disabledLayer
 */
void VSimulationFacade::enableLayer(unsigned int disabledLayer) noexcept {

}

/**
 * @param layer
 * @param material
 */
void VSimulationFacade::setMaterial(unsigned int layer, const VCloth &material) noexcept {

}

void VSimulationFacade::draw() noexcept {
    m_pGraphicsViewer->render();
}

/**
 * @param pressure
 */
void VSimulationFacade::setExternalPressure(double pressure) noexcept {

}

/**
 * @param diameter
 */
void VSimulationFacade::setInjectionDiameter(double diameter) noexcept {

}

/**
 * @param diameter
 */
void VSimulationFacade::setVacuumDiameter(double diameter) noexcept {

}

/**
 * @param viscosity
 */
void VSimulationFacade::setViscosity(double viscosity) noexcept {

}

void VSimulationFacade::newModel() noexcept {

}

/**
 * @param filename
 */
void VSimulationFacade::loadFromFile(const QString &filename) noexcept {

}
