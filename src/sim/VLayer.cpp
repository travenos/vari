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
VLayer::VLayer(std::vector<VSimNode::ptr> &nodes, std::vector<VSimTriangle::ptr> &triangles, const VCloth& material):
    m_nodes(nodes),
    m_triangles(triangles),
    m_pMaterial(new VCloth(material))

{

}

/**
 * @param visible
 */
void VLayer::setVisible(bool visible) noexcept
{
    for (auto &node : m_nodes)
        node->setVisible(visible);
    for (auto &triangle : m_triangles)
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
std::vector<VSimNode::ptr> &VLayer::getNodes() noexcept
{
    return m_nodes;
}

/**
 * @return vector<&VTriangle::ptr>
 */
std::vector<VSimTriangle::ptr> &VLayer::getTriangles() noexcept
{
    return m_triangles;
}

void VLayer::setMateial(const VCloth &material) noexcept
{
    (*m_pMaterial) = material;
}

void VLayer::reset() noexcept
{
    for (auto &node : m_nodes)
        node->reset();
    for (auto &triangle : m_triangles)
        triangle->reset();
}
