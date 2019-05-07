/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VLayersProcessor.h"

/**
 * VLayersProcessor implementation
 */


VLayersProcessor::VLayersProcessor():
    m_layerNextId(0),
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
    builder->setLayerId(m_layerNextId);
    builder->setNodeStartId(m_nodeNextId);
    builder->setTriangleStartId(m_triangleNextId);
    VLayer::ptr p_newLayer = builder->build();
    m_layerNextId = builder->getLayerId() + 1u;
    m_nodeNextId = builder->getNodeMaxId() + 1u;
    m_triangleNextId = builder->getTriangleMaxId() + 1u;
    m_layers.push_front(p_newLayer);
    putOnTop(0);
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
    updateNextIds();
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
            m_layersConnected = false;
        }
    }
    else if (m_layers.at(layer)->isActive())
    {
        m_layers.at(layer)->disconnect();
        m_layers.at(layer)->reset();
    }
    m_layers.at(layer)->markActive(enable);
    sortLayers(layer);
    updateActiveElementsVectors();
    emit layerEnabled(layer, enable);
    emit layerVisibilityChanged(layer, isLayerVisible(layer));
}

void VLayersProcessor::setMaterial(uint layer, const VCloth& material) 
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    std::lock_guard<std::mutex> lockT(*m_pTrianglesLock);
    double oldHeight = m_layers.at(layer)->getMaterial()->cavityHeight;
    m_layers.at(layer)->setMateial(material);
    if (oldHeight != material.cavityHeight)
    {
        sortLayers(layer);
    }
    emit materialChanged(layer);
}

void VLayersProcessor::moveUp(uint layer)
{
    if (layer > 0 && layer < getLayersNumber())
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        float otherLayerBaseZ{.0f};
        if (layer < getLayersNumber() - 1u)
        {
            float groundLayer = layer + 1u;
            while (!m_layers.at(groundLayer)->isActive())
            {
                ++groundLayer;
                if (groundLayer >= getLayersNumber() )
                {
                    break;
                }
            }
            if (groundLayer < getLayersNumber())
                otherLayerBaseZ = m_layers.at(groundLayer)->getMaxZ();
        }
        float otherLayerZ = otherLayerBaseZ + m_layers.at(layer - 1u)->getMaterial()->getHeight();
        float layerZ;
        m_layers.at(layer - 1u)->setVerticalPosition(otherLayerZ);
        if (m_layers.at(layer - 1u)->isActive())
            layerZ = m_layers.at(layer - 1u)->getMaxZ();
        else
            layerZ = otherLayerBaseZ;
        layerZ += m_layers.at(layer)->getMaterial()->getHeight();
        m_layers.at(layer)->setVerticalPosition(layerZ);
        std::swap(m_layers.at(layer), m_layers.at(layer - 1u));
        m_layersConnected = false;
    }
}

void VLayersProcessor::moveDown(uint layer)
{
    if (static_cast<int>(layer) < static_cast<int>(getLayersNumber()) - 1)
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        float layerBaseZ{.0f};
        if (static_cast<int>(layer) < static_cast<int>(getLayersNumber()) - 2)
        {
            float groundLayer = layer + 2u;
            while (!m_layers.at(groundLayer)->isActive())
            {
                ++groundLayer;
                if (groundLayer >= getLayersNumber() )
                {
                    break;
                }
            }
            if (groundLayer < getLayersNumber())
                layerBaseZ = m_layers.at(groundLayer)->getMaxZ();
        }
        float layerZ = layerBaseZ + m_layers.at(layer)->getMaterial()->getHeight();
        float otherLayerZ;
        m_layers.at(layer)->setVerticalPosition(layerZ);
        if (m_layers.at(layer)->isActive())
            otherLayerZ = m_layers.at(layer)->getMaxZ();
        else
            otherLayerZ = layerBaseZ;
        otherLayerZ += m_layers.at(layer + 1u)->getMaterial()->getHeight();
        m_layers.at(layer + 1u)->setVerticalPosition(otherLayerZ);
        std::swap(m_layers.at(layer), m_layers.at(layer + 1u));
        m_layersConnected = false;
    }
}

void VLayersProcessor::sort()
{
    if (getLayersNumber() > 0)
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        sortLayers(static_cast<uint>(getLayersNumber() - 1));
    }
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
    updateNextIds();
    emit layersCleared();
}

void VLayersProcessor::setLayers (const std::deque<VLayer::ptr> &layers)
{
    m_layers = layers;
    updateActiveElementsVectors();
    updateNextIds();
    emit layersLoaded();
}

void VLayersProcessor::updateNextIds()
{
    m_layerNextId = 0;
    m_nodeNextId = 0;
    m_triangleNextId = 0;
    for (auto &layer : m_layers)
    {
        uint layerId = layer->getId();
        uint nodeId = layer->getNodeMaxId();
        uint triangleId = layer->getTriangleMaxId();
        if (layerId > m_layerNextId)
            m_layerNextId = layerId;
        if (nodeId > m_nodeNextId)
            m_nodeNextId = nodeId;
        if (triangleId > m_triangleNextId)
            m_triangleNextId = triangleId;
    }
    if (getLayersNumber() > 0)
    {
        ++m_layerNextId;
        ++m_nodeNextId;
        ++m_triangleNextId;
    }
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

std::vector<std::vector<QPolygonF> > VLayersProcessor::getAllActivePolygons() const
{
    std::vector<std::vector<QPolygonF> > polygonsVector;
    polygonsVector.reserve(getActiveLayersNumber());
    for (auto & layer : m_layers)
    {
        if (layer->isActive())
        {
            polygonsVector.push_back(layer->getPolygons());
        }
    }
    return polygonsVector;
}

const std::vector<QPolygonF> & VLayersProcessor::getPolygons(uint layer)
{
    return m_layers.at(layer)->getPolygons();
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
                std::list<VLayer::ptr> layersToConnect;
                for(uint j = i + 1; j < m_layers.size(); ++j)
                {
                    if (isLayerEnabled(j))
                    {
                        layersToConnect.push_back(m_layers.at(j));
                    }
                }
                m_layers.at(i)->connectWith(layersToConnect);
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

const QString & VLayersProcessor::getLayerName(uint layer) const
{
    return m_layers.at(layer)->getName();
}

void VLayersProcessor::setLayerName(uint layer, const QString &name)
{
    m_layers.at(layer)->setName(name);
    emit nameChanged(layer);
}

void VLayersProcessor::putOnTop(uint layer)
{
    if (m_layers.size() > 1)
    {
        uint firstLayer = (layer != 0) ? 0 : 1;
        while (!m_layers.at(firstLayer)->isActive())
        {
            ++firstLayer;
            if (firstLayer == layer)
                ++firstLayer;
            if (firstLayer >= m_layers.size())
            {
                float z_max = m_layers.at(layer)->getMaterial()->getHeight();
                m_layers.at(layer)->setVerticalPosition(z_max);
                return;
            }
        }
        float z_max = m_layers.at(firstLayer)->getMaxZ();
        for (uint i = 0; i < m_layers.size(); ++i)
        {
            if (i != layer && m_layers.at(i)->isActive())
            {
                float z_max_i = m_layers.at(i)->getMaxZ();
                if (z_max_i > z_max)
                    z_max = z_max_i;
            }
        }
        z_max += m_layers.at(layer)->getMaterial()->getHeight();
        m_layers.at(layer)->setVerticalPosition(z_max);
    }
    else
    {
        float z_max = m_layers.at(layer)->getMaterial()->getHeight();
        m_layers.at(layer)->setVerticalPosition(z_max);
    }
}


void VLayersProcessor::sortLayers(uint fromLayer)
{
    if (fromLayer < getLayersNumber())
    {
        float layerZ{.0f};
        if (fromLayer < getLayersNumber() - 1u)
        {
            uint groundLayer = fromLayer + 1u;
            while (!m_layers.at(groundLayer)->isActive())
            {
                ++groundLayer;
                if (groundLayer >= m_layers.size())
                {
                    break;
                }
            }
            if (groundLayer < m_layers.size())
            {
                layerZ = m_layers.at(groundLayer)->getMaxZ();
            }
        }
        for (int i = static_cast<int>(fromLayer); i >= 0; --i)
        {
            if (m_layers.at(i)->isActive())
            {
                layerZ += m_layers.at(i)->getMaterial()->getHeight();
                m_layers.at(i)->setVerticalPosition(layerZ);
                if (i > 0)
                {
                    layerZ = m_layers.at(i)->getMaxZ();
                }
            }
        }
        m_layersConnected = false;
    }
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

VLayer::const_ptr VLayersProcessor::getLayer(uint layer) const
{
    if (layer < m_layers.size())
        return m_layers.at(layer);
    else
        return VLayer::const_ptr();
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

int VLayersProcessor::getLayerNumberById(uint layerId) const
{
    for (uint i = 0; i < m_layers.size(); ++i)
    {
        if (m_layers.at(i)->getId() == layerId)
            return i;
    }
    return -1;
}

uint VLayersProcessor::getLayerId(uint layer) const
{
    return m_layers.at(layer)->getId();
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

void VLayersProcessor::setInjectionPoint(const QVector2D &point, double diameter)
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    setPoint(point, diameter, VSimNode::INJECTION, VSimNode::VACUUM);
}

void VLayersProcessor::setVacuumPoint(const QVector2D &point, double diameter)
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

void VLayersProcessor::setPoint(const QVector2D &point, double diameter,
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
        for (auto &node : nearNodes)
        {
            node->setRole(setRole);
        }
    }
    nearestNode->setRole(setRole);
}
