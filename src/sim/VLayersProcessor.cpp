/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VLayersProcessor.h"

/**
 * VLayersProcessor implementation
 */


VLayersProcessor::VLayersProcessor():
    m_nodeNextId(0),
    m_triangleNextId(0),
    m_layersConnected(false)
{
    updateActiveElementsVectors();
}

size_t VLayersProcessor::getLayersNumber() const 
{
    return m_layers.size();
}

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
    m_layersConnected = false;
    emit layerAdded();
}

void VLayersProcessor::removeLayer(uint layer) 
{
    enableLayer(layer, false);
    auto it = m_layers.begin();
    std::advance(it, layer);
    if (it != m_layers.end())
    {
        m_layers.erase(it);
    }
    m_layersConnected = false;
    emit layerRemoved(layer);
}

void VLayersProcessor::setVisibleLayer(uint layer, bool visible) 
{
    m_layers.at(layer)->setVisible(visible);
    emit layerVisibilityChanged(layer, visible);
}

void VLayersProcessor::enableLayer(uint layer, bool enable) 
{
    if(enable)
    {
        if (!(m_layers.at(layer)->isActive()))
        {
            increasePositions(layer + 1);
            m_layersConnected = false;
        }
    }
    else if (m_layers.at(layer)->isActive())
    {
        m_layers.at(layer)->disconnect();
        m_layers.at(layer)->reset();
        decreasePositions(layer + 1);
    }
    m_layers.at(layer)->markActive(enable);
    updateActiveElementsVectors();
    emit layerEnabled(layer, enable);
    emit layerVisibilityChanged(layer, isLayerVisible(layer));
}

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

bool VLayersProcessor::areLayersConnected() const
{
    return m_layersConnected;
}

void VLayersProcessor::getActiveModelSize(QVector3D &size) const
{
    QVector3D min;
    QVector3D max;
    bool minMaxInitialized = false;
    for (auto &layer : m_layers)
    {
        if (layer->isActive())
        {
            QVector3D layerMin;
            QVector3D layerMax;
            layer->getConstrains(layerMin, layerMax);
            if (minMaxInitialized)
            {
                for (uint i = 0; i < 3; ++i)
                {
                    min[i] = std::min(min[i], layerMin[i]);
                    max[i] = std::max(max[i], layerMax[i]);
                }
            }
            else
            {
                min = layerMin;
                max = layerMax;
                minMaxInitialized = true;
            }
        }
    }
    if (minMaxInitialized)
        size = max - min;
    else
        size = QVector3D(0, 0, 0);
}

size_t VLayersProcessor::getActiveNodesNumber() const
{
    return m_pActiveNodes->size();
}

size_t VLayersProcessor::getActiveTrianglesNumber() const
{
    return m_pActiveTriangles->size();
}


void VLayersProcessor::createConnections()
{
    if (!areLayersConnected())
    {
        removeConnections();
        for(uint i = 0; i < m_layers.size(); ++i)
        {
            if (isLayerEnabled(i))
            {
                for(uint j = i + 1; j < m_layers.size(); ++j)
                {
                    if (isLayerEnabled(j))
                        m_layers.at(i)->connectWith(m_layers.at(j));
                }
            }
        }
        m_layersConnected = true;
    }
}


void VLayersProcessor::removeConnections()
{
    for (auto &layer : m_layers)
    {
        const VSimNode::map_ptr &nodes = layer->getNodes();
        for (auto &node : *nodes)
        {
            node.second->clearNeighbours(VSimNode::OTHER);
        }
    }
    m_layersConnected = false;
}

void VLayersProcessor::resetNodesVolumes()
{
    for (auto &layer : m_layers)
    {
        layer->resetNodesVolume();
    }
}

void VLayersProcessor::decreasePositions(uint fromLayer)  {
    //TODO find nearest ACTIVE layers to layer1 and layer2
}

void VLayersProcessor::increasePositions(uint fromLayer)  {
    //TODO find nearest ACTIVE layers to layer1 and layer2
}

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

std::vector<VLayer::const_ptr> VLayersProcessor::getLayers() const
{
    std::vector<VLayer::const_ptr> layers;
    layers.insert(layers.begin(), m_layers.begin(), m_layers.end());
    return layers;
}

void VLayersProcessor::cutLayer(const std::shared_ptr<const std::vector<uint> > &nodesIds, uint layer)
{
    m_layers.at(layer)->cut(nodesIds);
    updateActiveElementsVectors();
    m_layersConnected = false;
}

void VLayersProcessor::transformateLayer(const std::shared_ptr<const std::vector<std::pair<uint, QVector3D> > >
                    &nodesCoords, uint layer)
{
    m_layers.at(layer)->transformate(nodesCoords);
    updateActiveElementsVectors();
    m_layersConnected = false;
}

void VLayersProcessor::updateActiveElementsVectors() 
{
    std::vector<VSimNode::ptr> * activeNodes = new VSimNode::vector_t;
    std::vector<VSimTriangle::ptr> * activeTriangles = new VSimTriangle::vector_t;

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
            for (auto &node : *nodes)
                activeNodes->push_back(node.second);

            const VSimTriangle::list_ptr &triangles = layer->getTriangles();
            for (auto &triangle : *triangles)
                activeTriangles->push_back(triangle);
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
