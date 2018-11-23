/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VLayersProcessor.h"

/**
 * VLayersProcessor implementation
 * 
 * Typedef of std::vector<VLayer>
 */


/**
 * @param simulator
 */
VLayersProcessor::VLayersProcessor(VSimulator::ptr simulator) {

}

/**
 * @return size_type
 */
size_t VLayersProcessor::getLayersNumber() const {
    return null;
}

/**
 * @return size_type
 */
size_t VLayersProcessor::getInactiveLayersNumber() const {
    return null;
}

/**
 * @param builder
 */
void VLayersProcessor::addLayer(VLayerAbstractBuilder& builder) {

}

/**
 * @param layer
 */
void VLayersProcessor::removeLayer(unsigned int layer) {

}

/**
 * @param layer
 * @param visible
 */
void VLayersProcessor::setVisibleLayer(unsigned int layer, bool visible) {

}

/**
 * @param layer
 */
void VLayersProcessor::disableLayer(unsigned int layer) {

}

/**
 * @param disabledLayer
 */
void VLayersProcessor::enableLayer(unsigned int disabledLayer) {

}

/**
 * @param layer
 * @param material
 */
void VLayersProcessor::setMaterial(unsigned int layer, VMaterial& material) {

}

/**
 * @param parametres
 */
void VLayersProcessor::setParametres(VSimulationParametres& parametres) {

}

/**
 * @return double
 */
double VLayersProcessor::getMedianDistance() const {
    return 0.0;
}

void VLayersProcessor::reset() {

}

void VLayersProcessor::clear() {

}

/**
 * @param layer1
 * @param layer2
 */
void VLayersProcessor::createConnections(unsigned int layer1, unsigned int layer2) {

}

/**
 * @param layer1
 * @param layer2
 */
void VLayersProcessor::removeConnections(unsigned int layer1, unsigned int layer2) {

}

/**
 * @param layer
 */
void VLayersProcessor::decreasePosition(unsigned int layer) {

}

/**
 * @param layer
 */
void VLayersProcessor::increasePosition(unsigned int layer) {

}

/**
 * @param layer
 */
void VLayersProcessor::putOnTop(unsigned int layer) {

}
