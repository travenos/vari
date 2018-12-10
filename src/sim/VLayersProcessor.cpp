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
VLayersProcessor::VLayersProcessor()
{
    updateActiveElementsVectors();
}

/**
 * @return size_type
 */
size_t VLayersProcessor::getLayersNumber() const 
{
    return m_layers.size();
}

/**
 * @return size_type
 */
size_t VLayersProcessor::getActiveLayersNumber() const 
{
    size_t number = 0;
    for (auto &layer : m_layers)
    {
        if (layer->isActive())
            ++number;
    }
    return number;
}

/**
 * @return size_type
 */
size_t VLayersProcessor::getInactiveLayersNumber() const 
{
    size_t number = 0;
    for (auto &layer : m_layers)
    {
        if (!(layer->isActive()))
            ++number;
    }
    return number;
}

/**
 * @param builder
 */
void VLayersProcessor::addLayer(VLayerAbstractBuilder *builder) 
{
    VLayer::ptr p_newLayer = builder->build();
    //TODO put the layer on the highest position
    m_layers.push_back(p_newLayer);
    updateActiveElementsVectors();
}

/**
 * @param layer
 */
void VLayersProcessor::removeLayer(unsigned int layer) 
{
    enableLayer(layer, false);
    auto it = m_layers.begin();
    std::advance(it, layer);
    if (it != m_layers.end())
    {
        m_layers.erase(it);
    }
}

/**
 * @param layer
 * @param visible
 */
void VLayersProcessor::setVisibleLayer(unsigned int layer, bool visible) 
{
    m_layers.at(layer)->setVisible(visible);
}

/**
 * @param disabledLayer
 */
void VLayersProcessor::enableLayer(unsigned int layer, bool enable) 
{
    if(enable)
    {
        if (!(m_layers.at(layer)->isActive()))
        {
            removeConnections(layer - 1, layer + 1);
            createConnections(layer - 1, layer);
            createConnections(layer, layer + 1);
            increasePositions(layer + 1);
        }
    }
    else if (m_layers.at(layer)->isActive())
    {
        m_layers.at(layer)->reset();
        removeConnections(layer - 1, layer);
        removeConnections(layer, layer + 1);
        decreasePositions(layer + 1);
        createConnections(layer - 1, layer + 1);
    }
    m_layers.at(layer)->markActive(enable);
    updateActiveElementsVectors();
}

/**
 * @param layer
 * @param material
 */
void VLayersProcessor::setMaterial(unsigned int layer, const VCloth& material) 
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    std::lock_guard<std::mutex> lockT(*m_pTrianglesLock);
    m_layers.at(layer)->setMateial(material);
}

void VLayersProcessor::reset() 
{
    for (auto &layer : m_layers)
        layer->reset();
}

void VLayersProcessor::clear() 
{
    m_layers.clear();
    updateActiveElementsVectors();
}

VCloth::const_ptr VLayersProcessor::getMaterial(unsigned int layer) const 
{
    return m_layers.at(layer)->getMaterial();
}

bool VLayersProcessor::isLayerVisible(unsigned int layer) const 
{
    return m_layers.at(layer)->isVisible();
}

bool VLayersProcessor::isLayerEnabled(unsigned int layer) const 
{
    return m_layers.at(layer)->isActive();
}

/**
 * @param layer1
 * @param layer2
 */
void VLayersProcessor::createConnections(unsigned int layer1, unsigned int layer2)  {
    //TODO find nearest ACTIVE layers to layer1 and layer2
}

/**
 * @param layer1
 * @param layer2
 */
void VLayersProcessor::removeConnections(unsigned int layer1, unsigned int layer2)  {
    //TODO find nearest ACTIVE layers to layer1 and layer2
}

/**
 * @param layer
 */
void VLayersProcessor::decreasePositions(unsigned int fromLayer)  {
    //TODO find nearest ACTIVE layers to layer1 and layer2
}

/**
 * @param layer
 */
void VLayersProcessor::increasePositions(unsigned int fromLayer)  {
    //TODO find nearest ACTIVE layers to layer1 and layer2
}

/**
 * @param layer
 */
void VLayersProcessor::putOnTop(unsigned int layer)  {

}

const VSimNode::const_vector_ptr &VLayersProcessor::getActiveNodes() const 
{
    return m_pActiveNodes;
}
const VSimTriangle::const_vector_ptr &VLayersProcessor::getActiveTriangles() const 
{
    return m_pActiveTriangles;
}

void VLayersProcessor::updateActiveElementsVectors() 
{
    std::vector<VSimNode::ptr> * activeNodes = new std::vector<VSimNode::ptr>;
    std::vector<VSimTriangle::ptr> * activeTriangles = new std::vector<VSimTriangle::ptr>;
    size_t nodesSize = 0;
    size_t trianglesSize = 0;
    for (auto &layer : m_layers)
    {
        if (layer->isActive())
        {
            nodesSize += layer->getNodesNumber();
            trianglesSize += layer->getTrianglesNumber();
        }
    }
    activeNodes->reserve(nodesSize);
    activeTriangles->reserve(trianglesSize);
    for (auto &layer : m_layers)
    {
        if (layer->isActive())
        {
            VSimNode::vector_ptr nodes = layer->getNodes();
            activeNodes->insert(activeNodes->end(), nodes->begin(), nodes->end());
            VSimTriangle::vector_ptr triangles = layer->getTriangles();
            activeTriangles->insert(activeTriangles->end(), triangles->begin(), triangles->end());
        }
    }
    m_pActiveNodes.reset(activeNodes);
    m_pActiveTriangles.reset(activeTriangles);
}

void VLayersProcessor::setInjectionPoint(const QVector3D &point, float diameter)
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    m_layers.back()->setInjectionPoint(point, diameter);
}

void VLayersProcessor::setVacuumPoint(const QVector3D &point, float diameter)
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    m_layers.back()->setVacuumPoint(point, diameter);
}
