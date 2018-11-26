/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VSimulationFacade.h"

/**
 * VSimulationFacade implementation
 */


VSimulationFacade::VSimulationFacade(){

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
