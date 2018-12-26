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
VLayersProcessor::VLayersProcessor():
    m_nodeNextId(0),
    m_triangleNextId(0)
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
    builder->setNodeStartId(m_nodeNextId);
    builder->setTriangleStartId(m_triangleNextId);
    VLayer::ptr p_newLayer = builder->build();
    m_nodeNextId = builder->getNodeMaxId() + 1;
    m_triangleNextId = builder->getTriangleMaxId() + 1;
    //TODO put the layer on the highest position
    m_layers.push_back(p_newLayer);
    updateActiveElementsVectors();
    emit layerAdded();
}

/**
 * @param layer
 */
void VLayersProcessor::removeLayer(uint layer) 
{
    enableLayer(layer, false);
    auto it = m_layers.begin();
    std::advance(it, layer);
    if (it != m_layers.end())
    {
        m_layers.erase(it);
    }
    emit layerRemoved(layer);
}

/**
 * @param layer
 * @param visible
 */
void VLayersProcessor::setVisibleLayer(uint layer, bool visible) 
{
    m_layers.at(layer)->setVisible(visible);
    emit layerVisibilityChanged(layer, visible);
}

/**
 * @param disabledLayer
 */
void VLayersProcessor::enableLayer(uint layer, bool enable) 
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
    emit layerEnabled(layer, enable);
    emit layerVisibilityChanged(layer, isLayerVisible(layer));
}

/**
 * @param layer
 * @param material
 */
void VLayersProcessor::setMaterial(uint layer, const VCloth& material) 
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    std::lock_guard<std::mutex> lockT(*m_pTrianglesLock);
    m_layers.at(layer)->setMateial(material);
    emit materialChanged(layer);
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
    m_nodeNextId = 0;
    m_triangleNextId = 0;
    emit layersCleared();
}

void VLayersProcessor::setLayers (const std::vector<VLayer::ptr> &layers)
{
    m_layers = layers;
    updateActiveElementsVectors();
    updateNextIds();
    emit layersLoaded();
}

void VLayersProcessor::updateNextIds()
{
    m_nodeNextId = 0;
    m_triangleNextId = 0;
    for (auto &layer : m_layers)
    {
        uint nodeId = layer->getNodeMaxId();
        uint triangleId = layer->getTriangleMaxId();
        if (nodeId > m_nodeNextId)
            m_nodeNextId = nodeId;
        if (triangleId > m_triangleNextId)
            m_triangleNextId = triangleId;
    }
    ++m_nodeNextId;
    ++m_triangleNextId;
}

VCloth::const_ptr VLayersProcessor::getMaterial(uint layer) const 
{
    return m_layers.at(layer)->getMaterial();
}

bool VLayersProcessor::isLayerVisible(uint layer) const 
{
    return m_layers.at(layer)->isVisible();
}

bool VLayersProcessor::isLayerEnabled(uint layer) const 
{
    return m_layers.at(layer)->isActive();
}

void VLayersProcessor::getActiveModelSize(QVector3D &size) const
{
    if (m_pActiveNodes->size() > 0)
    {
        const QVector3D &firstNode = m_pActiveNodes->at(0)->getPosition();
        float minX = firstNode.x();
        float maxX = firstNode.x();
        float minY = firstNode.y();
        float maxY = firstNode.y();
        float minZ = firstNode.z();
        float maxZ = firstNode.z();
        for(auto &node : *m_pActiveNodes)
        {
            const QVector3D &pos = node->getPosition();
            if (pos.x() < minX)
                minX = pos.x();
            else if (pos.x() > maxX)
                maxX = pos.x();
            if (pos.y() < minY)
                minY = pos.y();
            else if (pos.y() > maxY)
                maxY = pos.y();
            if (pos.z() < minZ)
                minZ = pos.z();
            else if (pos.z() > maxZ)
                maxZ = pos.z();
        }
        size = QVector3D(maxX - minX, maxY - minY, maxZ - minZ);
    }
    else
    {
        size = QVector3D(0, 0, 0);
    }
}

size_t VLayersProcessor::getActiveNodesNumber() const
{
    return m_pActiveNodes->size();
}

size_t VLayersProcessor::getActiveTrianglesNumber() const
{
    return m_pActiveTriangles->size();
}

/**
 * @param layer1
 * @param layer2
 */
void VLayersProcessor::createConnections(uint layer1, uint layer2)  {
    //TODO find nearest ACTIVE layers to layer1 and layer2
}

/**
 * @param layer1
 * @param layer2
 */
void VLayersProcessor::removeConnections(uint layer1, uint layer2)  {
    //TODO find nearest ACTIVE layers to layer1 and layer2
}

/**
 * @param layer
 */
void VLayersProcessor::decreasePositions(uint fromLayer)  {
    //TODO find nearest ACTIVE layers to layer1 and layer2
}

/**
 * @param layer
 */
void VLayersProcessor::increasePositions(uint fromLayer)  {
    //TODO find nearest ACTIVE layers to layer1 and layer2
}

/**
 * @param layer
 */
void VLayersProcessor::putOnTop(uint layer)  {

}

const VSimNode::const_vector_ptr &VLayersProcessor::getActiveNodes() const 
{
    return m_pActiveNodes;
}
const VSimTriangle::const_vector_ptr &VLayersProcessor::getActiveTriangles() const 
{
    return m_pActiveTriangles;
}

VSimNode::const_map_ptr VLayersProcessor::getLayerNodes(uint layer) const
{
    return m_layers.at(layer)->getNodes();
}

VSimTriangle::const_list_ptr VLayersProcessor::getLayerTriangles(uint layer) const
{
    return m_layers.at(layer)->getTriangles();
}

size_t VLayersProcessor::getLayerNodesNumber(uint layer) const
{
    return m_layers.at(layer)->getNodesNumber();
}

size_t VLayersProcessor::getLayerTrianglesNumber(uint layer) const
{
    return m_layers.at(layer)->getTrianglesNumber();
}

void VLayersProcessor::cutLayer(const VGraphicsViewer::const_uint_vect_ptr &nodesIds, uint layer)
{
    m_layers.at(layer)->cut(nodesIds);
    updateActiveElementsVectors();
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
            const VSimNode::map_ptr &nodes = layer->getNodes();
            activeNodes->reserve(activeNodes->size() + nodes->size());
            for (auto &node : *nodes)
            {
                activeNodes->push_back(node.second);
            }
            const VSimTriangle::list_ptr &triangles = layer->getTriangles();
            activeTriangles->insert(activeTriangles->end(), triangles->begin(), triangles->end());
        }
    }
    m_pActiveNodes.reset(activeNodes);
    m_pActiveTriangles.reset(activeTriangles);
}

void VLayersProcessor::setInjectionPoint(const QVector3D &point, double diameter)
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    setPoint(point, diameter, VSimNode::INJECTION, VSimNode::VACUUM);
}

void VLayersProcessor::setVacuumPoint(const QVector3D &point, double diameter)
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    setPoint(point, diameter, VSimNode::VACUUM, VSimNode::INJECTION);
}

uint VLayersProcessor::getNodeMinId(uint layer) const
{
    return m_layers.at(layer)->getNodeMinId();
}

uint VLayersProcessor::getNodeMaxId(uint layer) const
{
    return m_layers.at(layer)->getNodeMaxId();
}

uint VLayersProcessor::getTriangleMinId(uint layer) const
{
    return m_layers.at(layer)->getTriangleMinId();
}

uint VLayersProcessor::getTriangleMaxId(uint layer) const
{
    return m_layers.at(layer)->getTriangleMaxId();
}

void VLayersProcessor::setPoint(const QVector3D &point, double diameter,
                      VSimNode::VNodeRole setRole, VSimNode::VNodeRole anotherRole)
{
    if (m_pActiveNodes->size() == 0)
        return;
    double radius = diameter / 2;
    double distance;
    VSimNode::ptr nearestNode = m_pActiveNodes->front();
    std::vector<VSimNode::ptr> nearNodes;
    double minDistance = nearestNode->getDistance(point);
    for (auto &node : *m_pActiveNodes)
    {
        if (node->getRole() != anotherRole)
            node->setRole(VSimNode::NORMAL);
        distance = node->getDistance(point);
        if (distance < minDistance)
        {
            minDistance = distance;
            nearestNode = node;
        }
        if (distance <= radius)
        {
            nearNodes.push_back(node);
        }
    }
    if (nearNodes.size() > 0)
    {
        double height = nearestNode->getCavityHeight() / 2;
        for (auto &node : nearNodes)
        {
            double z = node->getPosition().z();
            if (abs(z - point.z()) <= height)
                node->setRole(setRole);
        }
    }
    nearestNode->setRole(setRole);
}
