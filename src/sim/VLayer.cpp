/**
 * Project VARI
 * @author Alexey Barashkov
 */
#ifdef DEBUG_MODE
#include <QDebug>
#endif

#include "VLayer.h"

/**
 * VLayer implementation
 */


/**
 * @param nodes
 * @param triangles
 * @param material
 */
VLayer::VLayer(const VSimNode::map_ptr &nodes,
               const VSimTriangle::list_ptr &triangles,
               const VCloth::ptr &material):
    m_pNodes(nodes),
    m_pTriangles(triangles),
    m_pMaterial(material),
    m_visible(true),
    m_wasVisible(true),
    m_active(true),
    m_nodeMinId(0),
    m_nodeMaxId(0),
    m_triangleMinId(0),
    m_triangleMaxId(0)
{
}

VLayer::~VLayer()
{
    #ifdef DEBUG_MODE
        qInfo() << "VLayer destroyed";
    #endif
}

/**
 * @param visible
 */
void VLayer::setVisible(bool visible) 
{
    p_setVisible(visible);
    m_wasVisible = visible;
}

void VLayer::p_setVisible(bool visible) 
{
    for (auto &node : *m_pNodes)
        node.second->setVisible(visible);
    for (auto &triangle : *m_pTriangles)
        triangle->setVisible(visible);
    m_visible = visible;
}

/**
 * @return bool
 */
bool VLayer::isVisible() const 
{
    return m_visible;
}

void VLayer::markActive(bool active) 
{
    if (active)
    {
        p_setVisible(m_wasVisible);
    }
    else
    {
        p_setVisible(false);
    }
    m_active = active;
}

/**
 * @return bool
 */
bool VLayer::isActive() const 
{
    return m_active;
}

/**
 * @return vector<&VNode::ptr>
 */
const VSimNode::map_ptr &VLayer::getNodes() const
{
    return m_pNodes;
}

const VSimTriangle::list_ptr &VLayer::getTriangles() const
{
    return m_pTriangles;
}

void VLayer::setMateial(const VCloth &material) 
{
    (*m_pMaterial) = material;
}

void VLayer::reset() 
{
    for (auto &node : *m_pNodes)
        node.second->reset();
    for (auto &triangle : *m_pTriangles)
        triangle->reset();
}

size_t VLayer::getNodesNumber() const 
{
    return m_pNodes->size();
}

size_t VLayer::getTrianglesNumber() const 
{
    return m_pTriangles->size();
}

VCloth::const_ptr VLayer::getMaterial() const 
{
    return m_pMaterial;
}

void VLayer::setMinMaxIds(uint nodeMinId, uint nodeMaxId, uint tiangleMinId, uint triangleMaxId)
{
    m_nodeMinId = nodeMinId;
    m_nodeMaxId = nodeMaxId;
    m_triangleMinId = tiangleMinId;
    m_triangleMaxId = triangleMaxId;
}

uint VLayer::getNodeMinId() const
{
    return m_nodeMinId;
}

uint VLayer::getNodeMaxId() const
{
    return m_nodeMaxId;
}

uint VLayer::getTriangleMinId() const
{
    return m_triangleMinId;
}

uint VLayer::getTriangleMaxId() const
{
    return m_triangleMaxId;
}

void VLayer::cut(const std::shared_ptr<const std::vector<uint> > &nodesIds)
{
    VSimNode::map_ptr p_remainingNodesMap(new VSimNode::map_t);
    p_remainingNodesMap->reserve(nodesIds->size());
    for (uint id : *nodesIds)
    {
        auto node = m_pNodes->find(id);
        if (node != m_pNodes->end())
        {
            p_remainingNodesMap->insert(*node);
            m_pNodes->erase(node);
        }
    }
    for (auto &node : *m_pNodes)
        node.second->isolateAndMarkForRemove();
    auto it = m_pTriangles->begin();
    while (it != m_pTriangles->end())
    {
        if ((*it)->isMarkedForRemove())
        {
            m_pTriangles->erase(it++);
        }
        else
            ++it;
    }
    m_pNodes = p_remainingNodesMap;
}

void VLayer::transformate(const std::shared_ptr<const std::vector<std::pair<uint, QVector3D> > >
                    &nodesCoords)
{
    for (auto &pair : *nodesCoords)
    {
        auto node_pair = m_pNodes->find(pair.first);
        if (node_pair != m_pNodes->end())
        {
            node_pair->second->setPosition(pair.second);
        }
    }
}
