/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VSimulationFacade.h"

/**
 * VSimulationFacade implementation
 */


void VSimulationFacade::VSimulationFacade() {

}

void VSimulationFacade::startSimulation() {

}

void VSimulationFacade::stopSimulation() {

}

void VSimulationFacade::resetSimulation() {

}

/**
 * @return int
 */
int VSimulationFacade::getLayersNumber() {
    return 0;
}

/**
 * @return int
 */
int VSimulationFacade::getInactiveLayersNumber() {
    return 0;
}

/**
 * @param layer
 * @param visible
 */
void VSimulationFacade::setVisible(unsigned int layer, bool visible) {

}

/**
 * @param layer
 */
void VSimulationFacade::removeLayer(unsigned int layer) {

}

/**
 * @param layer
 */
void VSimulationFacade::disableLayer(unsigned int layer) {

}

/**
 * @param disabledLayer
 */
void VSimulationFacade::enableLayer(unsigned int disabledLayer) {

}

/**
 * @param layer
 * @param material
 */
void VSimulationFacade::setMaterial(unsigned int layer, VMaterial& material) {

}

void VSimulationFacade::draw() {

}

/**
 * @param pressure
 */
void VSimulationFacade::setExternalPressure(double pressure) {

}

/**
 * @param diameter
 */
void VSimulationFacade::setInjectionDiameter(double diameter) {

}

/**
 * @param diameter
 */
void VSimulationFacade::setVacuumDiameter(double diameter) {

}

/**
 * @param viscosity
 */
void VSimulationFacade::setViscosity(double viscosity) {

}

void VSimulationFacade::newModel() {

}

/**
 * @param filename
 */
void VSimulationFacade::loadFromFile(QString filename) {

}