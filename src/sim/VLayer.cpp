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
    m_hasInjectionPoint(false),
    m_hasVacuumPoint(false),
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

bool VLayer::getInjectionPointInfo(QVector3D &point, double &diameter) const
{
    if (m_hasInjectionPoint)
    {
        point = m_injectionPoint;
        diameter = m_injectionDiameter;
        return true;
    }
    else
        return false;
}

bool VLayer::getVacuumPointInfo(QVector3D &point, double &diameter) const
{
    if (m_hasVacuumPoint)
    {
        point = m_vacuumPoint;
        diameter = m_vacuumDiameter;
        return true;
    }
    else
        return false;
}

void VLayer::setInjectionPoint(const QVector3D &point, double diameter)
{
    setPoint(point, diameter, VSimNode::INJECTION, VSimNode::VACUUM);
}

void VLayer::setVacuumPoint(const QVector3D &point, double diameter)
{
    setPoint(point, diameter, VSimNode::VACUUM, VSimNode::INJECTION);
}

void VLayer::setPoint(const QVector3D &point, double diameter,
                      VSimNode::VNodeRole setRole, VSimNode::VNodeRole anotherRole)
{
    if (m_pNodes->size() == 0)
        return;
    m_hasInjectionPoint = true;
    m_injectionPoint = point;
    m_injectionDiameter = diameter;
    double radius = diameter / 2;
    double distance;
    VSimNode::ptr nearestNode = m_pNodes->front();
    double minDistance = nearestNode->getDistance(point);
    bool foundNear = false;
    for (VSimNode::ptr &node : *m_pNodes)
    {
        distance = node->getDistance(point);
        if (distance > radius)
        {
            if (node->getRole() != anotherRole)
                node->setRole(VSimNode::NORMAL);
            if (!foundNear && distance < minDistance)
            {
                minDistance = distance;
                nearestNode = node;
            }
        }
        else
        {
            node->setRole(setRole);
            foundNear = true;
        }
    }
    if (!foundNear)
        nearestNode->setRole(setRole);
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
