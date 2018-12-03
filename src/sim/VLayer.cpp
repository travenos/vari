/**
 * Project VARI
 * @author Alexey Barashkov
 */


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
    m_pMaterial(material)
{

}

/**
 * @param visible
 */
void VLayer::setVisible(bool visible) noexcept
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
bool VLayer::isVisible() const noexcept {
    return m_visible;
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
