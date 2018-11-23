/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VSimTriangle.h"

/**
 * VSimTriangle implementation
 */


/**
 * @param p_material
 * @param p_node0
 * @param p_node1
 * @param p_node2
 */
VSimTriangle::VSimTriangle(VMaterial::const_ptr p_material,
                           VSimNode::const_ptr p_node0,
                           VSimNode::const_ptr p_node1,
                           VSimNode::const_ptr p_node2) :
    VSimElement(p_material),
    m_pNodes{p_node0, p_node1, p_node2}
{

}

/**
 * @return std::vector<QVector3D>
 */
std::vector<QVector3D> VSimTriangle::getVertices() const{
    return {
        m_pNodes[0]->getPosition(),
        m_pNodes[1]->getPosition(),
        m_pNodes[2]->getPosition()
    };
}

void VSimTriangle::updateColor() {

}

/**
 * @return QColor&
 */
const QColor &VSimTriangle::getColor() const {
    return m_color;
}
