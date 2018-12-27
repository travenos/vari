/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include "VGraphicsLayer.h"

#include "VGraphicsNode.h"
#include "VGraphicsTriangle.h"

VGraphicsLayer::VGraphicsLayer(const VLayer::const_ptr &simLayer, uint number) :
    m_number(number)
{
    const VSimNode::map_ptr &simNodes = simLayer->getNodes();
    m_graphicsNodes.reserve(simNodes->size());
    for (auto &simNode : *simNodes)
        addNode(simNode.second);
    const VSimTriangle::list_ptr &simTriangles = simLayer->getTriangles();
    m_graphicsTriangles.reserve(simTriangles->size());
    for (auto &simTriangle : *simTriangles)
        addTriangle(simTriangle);
}

VGraphicsLayer::~VGraphicsLayer()
{

}

inline void VGraphicsLayer::addNode(const VSimNode::const_ptr &simNode)
{
    VGraphicsNode * node = new VGraphicsNode(simNode);
    m_graphicsNodes.push_back(node);
    addChild(node);
}

inline void VGraphicsLayer::addTriangle(const VSimTriangle::const_ptr &simTriangle)
{
    VGraphicsTriangle * triangle = new VGraphicsTriangle(simTriangle);
    m_graphicsTriangles.push_back(triangle);
    addChild(triangle);
}

void VGraphicsLayer::clearNodes()
{
    for (auto node : m_graphicsNodes)
        removeChild(node);
    m_graphicsNodes.clear();
}

void VGraphicsLayer::clearTriangles()
{
    for (auto triangle : m_graphicsTriangles)
        removeChild(triangle);
    m_graphicsTriangles.clear();
}

void VGraphicsLayer::reserveNodes(size_t n)
{
    m_graphicsNodes.reserve(n);
}

void VGraphicsLayer::reserveTriangles(size_t n)
{
    m_graphicsTriangles.reserve(n);
}

void VGraphicsLayer::showInjectionPoint()
{
    for (auto node : m_graphicsNodes)
        node->colorIfInjection();
    for (auto triangle : m_graphicsTriangles)
        triangle->colorIfInjection();
}

void VGraphicsLayer::showVacuumPoint()
{
    for (auto node : m_graphicsNodes)
        node->colorIfVacuum();
    for (auto triangle : m_graphicsTriangles)
        triangle->colorIfVacuum();
}

void VGraphicsLayer::updateNodeColors()
{
    for (auto &node : m_graphicsNodes)
        node->updateColor();
}

void VGraphicsLayer::updateTriangleColors()
{
    for (auto &triangle : m_graphicsTriangles)
        triangle->updateColor();
}

void VGraphicsLayer::updateVisibility()
{
    for (auto &node : m_graphicsNodes)
        node->updateVisibility();
    for (auto &triangle : m_graphicsTriangles)
        triangle->updateVisibility();
}

uint VGraphicsLayer::getNumber() const
{
    return m_number;
}
