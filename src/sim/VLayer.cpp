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
VLayer::VLayer(const VSimNode::vector_ptr &nodes,
               const VSimTriangle::vector_ptr &triangles,
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
        node->setVisible(visible);
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
const VSimNode::vector_ptr &VLayer::getNodes() 
{
    return m_pNodes;
}

/**
 * @return vector<&VTriangle::ptr>
 */
const VSimTriangle::vector_ptr &VLayer::getTriangles() 
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
        node->reset();
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
