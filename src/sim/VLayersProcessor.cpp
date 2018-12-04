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
VLayersProcessor::VLayersProcessor() {
    updateActiveElementsVectors();
}

/**
 * @return size_type
 */
size_t VLayersProcessor::getLayersNumber() const noexcept
{
    return m_layers.size();
}

/**
 * @return size_type
 */
size_t VLayersProcessor::getInactiveLayersNumber() const noexcept
{
    return m_inactiveLayers.size();
}

/**
 * @param builder
 */
void VLayersProcessor::addLayer(VLayerAbstractBuilder *builder) noexcept(false)
{
    VLayer::ptr p_newLayer = builder->build();
    //TODO put the layer on the highest position
    m_layers.push_back(p_newLayer);
    updateActiveElementsVectors();
}

/**
 * @param layer
 */
void VLayersProcessor::removeLayer(unsigned int layer) noexcept(false) {

}

/**
 * @param layer
 * @param visible
 */
void VLayersProcessor::setVisibleLayer(unsigned int layer, bool visible) noexcept(false) {

}

/**
 * @param layer
 */
void VLayersProcessor::disableLayer(unsigned int layer) noexcept(false) {

}

/**
 * @param disabledLayer
 */
void VLayersProcessor::enableLayer(unsigned int disabledLayer) noexcept(false) {

}

/**
 * @param layer
 * @param material
 */
void VLayersProcessor::setMaterial(unsigned int layer, const VCloth& material) noexcept(false)
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    std::lock_guard<std::mutex> lockT(*m_pTrianglesLock);
    m_layers[layer]->setMateial(material);
}

void VLayersProcessor::reset() noexcept {

}

void VLayersProcessor::clear() noexcept {

}

/**
 * @param layer1
 * @param layer2
 */
void VLayersProcessor::createConnections(unsigned int layer1, unsigned int layer2) noexcept(false) {

}

/**
 * @param layer1
 * @param layer2
 */
void VLayersProcessor::removeConnections(unsigned int layer1, unsigned int layer2) noexcept(false) {

}

/**
 * @param layer
 */
void VLayersProcessor::decreasePosition(unsigned int layer) noexcept(false) {

}

/**
 * @param layer
 */
void VLayersProcessor::increasePosition(unsigned int layer) noexcept(false) {

}

/**
 * @param layer
 */
void VLayersProcessor::putOnTop(unsigned int layer) noexcept(false) {

}

const VSimNode::const_vector_ptr &VLayersProcessor::getActiveNodes() const noexcept
{
    return m_pActiveNodes;
}
const VSimTriangle::const_vector_ptr &VLayersProcessor::getActiveTriangles() const noexcept
{
    return m_pActiveTriangles;
}

void VLayersProcessor::updateActiveElementsVectors() noexcept
{
    std::vector<VSimNode::ptr> * activeNodes = new std::vector<VSimNode::ptr>;
    std::vector<VSimTriangle::ptr> * activeTriangles = new std::vector<VSimTriangle::ptr>;
    size_t nodesSize = 0;
    size_t trianglesSize = 0;
    for (auto &layer : m_layers)
    {
        nodesSize += layer->getNodesNumber();
        trianglesSize += layer->getTrianglesNumber();
    }
    activeNodes->reserve(nodesSize);
    activeTriangles->reserve(trianglesSize);
    for (auto &layer : m_layers)
    {
        VSimNode::vector_ptr nodes = layer->getNodes();
        activeNodes->insert(activeNodes->end(), nodes->begin(), nodes->end());
        VSimTriangle::vector_ptr triangles = layer->getTriangles();
        activeTriangles->insert(activeTriangles->end(), triangles->begin(), triangles->end());
    }
    m_pActiveNodes.reset(activeNodes);
    m_pActiveTriangles.reset(activeTriangles);
}
