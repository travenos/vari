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
    m_active(true)
{

}

VLayer::~VLayer()
{
    #ifdef DEBUG_MODE
        qDebug() << "VLayer destroyed";
    #endif
}

/**
 * @param visible
 */
void VLayer::setVisible(bool visible) noexcept
{
    p_setVisible(visible);
    m_wasVisible = visible;
}

void VLayer::p_setVisible(bool visible) noexcept
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
bool VLayer::isVisible() const noexcept
{
    return m_visible;
}

void VLayer::markActive(bool active) noexcept
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
bool VLayer::isActive() const noexcept
{
    return m_active;
}

/**
 * @return vector<&VNode::ptr>
 */
const VSimNode::vector_ptr &VLayer::getNodes() noexcept
{
    return m_pNodes;
}

/**
 * @return vector<&VTriangle::ptr>
 */
const VSimTriangle::vector_ptr &VLayer::getTriangles() noexcept
{
    return m_pTriangles;
}

void VLayer::setMateial(const VCloth &material) noexcept
{
    (*m_pMaterial) = material;
}

void VLayer::reset() noexcept
{
    for (auto &node : *m_pNodes)
        node->reset();
    for (auto &triangle : *m_pTriangles)
        triangle->reset();
}

size_t VLayer::getNodesNumber() const noexcept
{
    return m_pNodes->size();
}

size_t VLayer::getTrianglesNumber() const noexcept
{
    return m_pTriangles->size();
}

VCloth::const_ptr VLayer::getMaterial() const noexcept
{
    return m_pMaterial;
}