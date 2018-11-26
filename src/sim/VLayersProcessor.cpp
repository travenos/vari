/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VLayersProcessor.h"

/**
 * VLayersProcessor implementation
 */


/**
 * @param simulator
 */
VLayersProcessor::VLayersProcessor(VSimulator::ptr simulator) {

}

/**
 * @return size_type
 */
size_t VLayersProcessor::getLayersNumber() const noexcept {

}

/**
 * @return size_type
 */
size_t VLayersProcessor::getInactiveLayersNumber() const noexcept {

}

/**
 * @param builder
 */
void VLayersProcessor::addLayer(VLayerAbstractBuilder& builder) noexcept {

}

/**
 * @param layer
 */
void VLayersProcessor::removeLayer(unsigned int layer) noexcept {

}

/**
 * @param layer
 * @param visible
 */
void VLayersProcessor::setVisibleLayer(unsigned int layer, bool visible) noexcept {

}

/**
 * @param layer
 */
void VLayersProcessor::disableLayer(unsigned int layer) noexcept {

}

/**
 * @param disabledLayer
 */
void VLayersProcessor::enableLayer(unsigned int disabledLayer) noexcept {

}

/**
 * @param layer
 * @param material
 */
void VLayersProcessor::setMaterial(unsigned int layer, VCloth& material) noexcept {

}

/**
 * @param parametres
 */
void VLayersProcessor::setParametres(VSimulationParametres& parametres) noexcept {

}

/**
 * @return double
 */
double VLayersProcessor::getMedianDistance() const noexcept {
    return 0.0;
}

void VLayersProcessor::reset() noexcept {

}

void VLayersProcessor::clear() noexcept {

}

/**
 * @param layer1
 * @param layer2
 */
void VLayersProcessor::createConnections(unsigned int layer1, unsigned int layer2) noexcept {

}

/**
 * @param layer1
 * @param layer2
 */
void VLayersProcessor::removeConnections(unsigned int layer1, unsigned int layer2) noexcept {

}

/**
 * @param layer
 */
void VLayersProcessor::decreasePosition(unsigned int layer) noexcept {

}

/**
 * @param layer
 */
void VLayersProcessor::increasePosition(unsigned int layer) noexcept {

}

/**
 * @param layer
 */
void VLayersProcessor::putOnTop(unsigned int layer) noexcept {

}
