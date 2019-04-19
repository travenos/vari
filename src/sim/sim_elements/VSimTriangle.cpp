/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif

#include "VSimTriangle.h"

/**
 * VSimTriangle implementation
 */

VSimTriangle::VSimTriangle(uint id, const VCloth::const_ptr &p_material,
                           const VSimNode::const_ptr &p_node0,
                           const VSimNode::const_ptr &p_node1,
                           const VSimNode::const_ptr &p_node2) :
    VSimElement(id, p_material),
    m_color(p_material->baseColor),
    m_pNodes({p_node0.get(), p_node1.get(), p_node2.get()})
{

}

VSimTriangle::VSimTriangle(uint id, const VCloth::const_ptr &p_material,
                           const VSimNode::const_ptr &p_node0,
                           const VSimNode::const_ptr &p_node1,
                           const VSimNode::const_ptr &p_node2,
                           const QColor &color) :
    VSimElement(id, p_material),
    m_color(color),
    m_pNodes({p_node0.get(), p_node1.get(), p_node2.get()})
{

}

VSimTriangle::~VSimTriangle()
{
    #ifdef DEBUG_MODE
        qInfo() << "VSimTriangle destroyed";
    #endif
}

void VSimTriangle::getVertices(QVector3D vertices[VERTICES_NUMBER]) const 
{
    for (uint i = 0; i < VERTICES_NUMBER; ++i)
        vertices[i] = m_pNodes[i]->getPosition();
}

QVector3D VSimTriangle::getVerticesId() const
{
    QVector3D ids;
    for (uint i = 0; i < VERTICES_NUMBER; ++i)
        ids[i] = m_pNodes[i]->getId();
    return ids;
}

void VSimTriangle::updateColor() 
{
    float filledPart = getFilledPart();
    m_color.setRgb(RGB_MAX * filledPart, RGB_MAX * (1-filledPart), 0);
}

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

bool VSimTriangle::isMarkedForRemove() const
{
    for (auto &node: m_pNodes)
        if (node->isMarkedForRemove())
            return true;
    return false;
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
    if (role == VSimNode::NORMAL)
    {
        for (auto &node: m_pNodes)
            if (node->getRole() != role)
                return false;
        return true;
    }
    else
    {
        for (auto &node: m_pNodes)
            if (node->getRole() == role)
                return true;
        return false;
    }
}
