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
VSimTriangle::VSimTriangle(const VCloth::const_ptr &p_material,
                           const VSimulationParametres::const_ptr &p_param,
                           const VSimNode::const_ptr &p_node0,
                           const VSimNode::const_ptr &p_node1,
                           const VSimNode::const_ptr &p_node2) :
    VSimElement(p_material, p_param),
    m_color(p_material->baseColor),
    m_pNodes({p_node0.get(), p_node1.get(), p_node2.get()})
{

}

/**
 * @return std::vector<QVector3D>
 */
void VSimTriangle::getVertices(QVector3D vertices[VERTICES_NUMBER]) const 
{
    for (unsigned int i = 0; i < VERTICES_NUMBER; ++i)
        vertices[i] = m_pNodes[i]->getPosition();
}

void VSimTriangle::updateColor() 
{
    float filledPart = getFilledPart();
    m_color.setRgb(RGB_MAX * filledPart, RGB_MAX * (1-filledPart), 0);
}

/**
 * @return QColor&
 */
const QColor &VSimTriangle::getColor() const 
{
    return m_color;
}

double VSimTriangle::getPressure() const
{
    double pressure = 0;
    for (size_t i = 0; i < VERTICES_NUMBER; ++i)
        pressure += m_pNodes[i]->getPressure();
    return pressure / VERTICES_NUMBER;
}

double VSimTriangle::getFilledPart() const
{
    double filledPart = 0;
    for (size_t i = 0; i < VERTICES_NUMBER; ++i)
        filledPart += m_pNodes[i]->getFilledPart();
    return filledPart / VERTICES_NUMBER;
}

void VSimTriangle::reset() 
{
    m_color = m_pMaterial->baseColor;
}

bool VSimTriangle::isInjection() const
{
    return isRole(VSimNode::INJECTION);
}

bool VSimTriangle::isVacuum() const
{
    return isRole(VSimNode::VACUUM);
}

bool VSimTriangle::isNormal() const
{
    return isRole(VSimNode::NORMAL);
}

bool VSimTriangle::isRole(VSimNode::VNodeRole role) const
{
    for (auto &node: m_pNodes)
        if (node->getRole() != role)
            return false;
    return true;
}
