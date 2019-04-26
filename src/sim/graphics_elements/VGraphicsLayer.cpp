/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif

#include <Inventor/nodes/SoTransform.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/misc/SoChildList.h>

#include "VGraphicsLayer.h"

#include "VGraphicsTriangle.h"

VGraphicsLayer::VGraphicsLayer(const VLayer::const_ptr &simLayer, float cubeSide) :
    m_id(simLayer->getId()),
    m_visible(false),
    m_pTransform(new SoTransform)
{
    addChild(m_pTransform);
    m_transformId = findChild(m_pTransform);
    m_pTransform->ref();
    const VSimNode::map_ptr &simNodes = simLayer->getNodes();
    reserveNodes(simNodes->size());
    for (auto &simNode : *simNodes)
        addNode(simNode.second, cubeSide);
    const VSimTriangle::list_ptr &simTriangles = simLayer->getTriangles();
    reserveTriangles(simTriangles->size());
    for (auto &simTriangle : *simTriangles)
        addTriangle(simTriangle);
}

VGraphicsLayer::~VGraphicsLayer()
{
    removeAllChildren();
    m_pTransform->unref();
    #ifdef DEBUG_MODE
        qInfo() << "VGraphicsLayer destroyed";
    #endif
}

inline void VGraphicsLayer::addNode(const VSimNode::const_ptr &simNode, float cubeSide)
{
    VGraphicsNode * node = new VGraphicsNode(simNode, cubeSide);
    addChild(node);
    int id = findChild(node);
    m_graphicsNodes.push_back(std::make_pair(id, node));
    if (node->isVisible())
        m_visible = true;
}

inline void VGraphicsLayer::addTriangle(const VSimTriangle::const_ptr &simTriangle)
{
    VGraphicsTriangle * triangle = new VGraphicsTriangle(simTriangle);
    addChild(triangle);
    int id = findChild(triangle);
    m_graphicsTriangles.push_back(std::make_pair(id, triangle));
    if (triangle->isVisible())
        m_visible = true;
}

void VGraphicsLayer::clearNodes()
{
    for (auto &node : m_graphicsNodes)
        removeChild(node.second);
    m_graphicsNodes.clear();
}

void VGraphicsLayer::clearTriangles()
{
    for (auto &triangle : m_graphicsTriangles)
        removeChild(triangle.second);
    m_graphicsTriangles.clear();
}

inline void VGraphicsLayer::reserveNodes(size_t n)
{
    m_graphicsNodes.reserve(n);
}

inline void VGraphicsLayer::reserveTriangles(size_t n)
{
    m_graphicsTriangles.reserve(n);
}

void VGraphicsLayer::showInjectionPoint()
{
    for (auto &node : m_graphicsNodes)
        node.second->colorIfInjection();
    for (auto &triangle : m_graphicsTriangles)
        triangle.second->colorIfInjection();
}

void VGraphicsLayer::showVacuumPoint()
{
    for (auto &node : m_graphicsNodes)
        node.second->colorIfVacuum();
    for (auto &triangle : m_graphicsTriangles)
        triangle.second->colorIfVacuum();
}

void VGraphicsLayer::updateNodeColors()
{
    for (auto &node : m_graphicsNodes)
        node.second->updateColor();
}

void VGraphicsLayer::updateTriangleColors()
{
    for (auto &triangle : m_graphicsTriangles)
        triangle.second->updateColor();
}

void VGraphicsLayer::updateVisibility()
{
    m_visible = false;
    for (auto &node : m_graphicsNodes)
    {
        node.second->updateVisibility();
        if (node.second->isVisible())
            m_visible = true;
    }
    for (auto &triangle : m_graphicsTriangles)
    {
        triangle.second->updateVisibility();
        if (triangle.second->isVisible())
            m_visible = true;
    }
}

void VGraphicsLayer::updatePosition()
{
    resetTransform();
    for (auto &node : m_graphicsNodes)
    {
        node.second->updatePosition();
    }
    for (auto &triangle : m_graphicsTriangles)
    {
        triangle.second->updatePosition();
    }
}

void VGraphicsLayer::resetTransform()
{
    SoNode * currentTransform = getChild(m_transformId);
    if (currentTransform != m_pTransform)
    {
        this->replaceChild(currentTransform, m_pTransform);
    }
}

uint VGraphicsLayer::getId() const
{
    return m_id;
}

bool VGraphicsLayer::isVisible() const
{
    return m_visible;
}

const SoTransform * VGraphicsLayer::getTransform() const
{
    return m_pTransform;
}

int VGraphicsLayer::getTransformId() const
{
    return m_transformId;
}

void VGraphicsLayer::setCubeSide(float side)
{
    for (auto &node : m_graphicsNodes)
    {
        node.second->setCubeSide(side);
    }
}

float VGraphicsLayer::getMinCubeSide(bool *sidesEqual) const
{
    if (m_graphicsNodes.size() > 0)
    {
        bool allEqual = true;
        auto nodeIt = m_graphicsNodes.begin();
        float minSide = nodeIt->second->getCubeSide();
        std::advance(nodeIt, 1);
        for (;nodeIt < m_graphicsNodes.end(); ++nodeIt)
        {
            float nodeSide = nodeIt->second->getCubeSide();
            if (nodeSide < minSide)
            {
                minSide = nodeSide;
                allEqual = false;
            }
        }
        if (sidesEqual != nullptr)
            *sidesEqual = allEqual;
        return minSide;
    }
    else
    {
        if (sidesEqual != nullptr)
            *sidesEqual = false;
        return VGraphicsNode::DEFAULT_CUBE_SIDE;
    }
}

float VGraphicsLayer::getFirstCubeSide() const
{
    if (m_graphicsNodes.size() > 0)
    {
        return m_graphicsNodes.begin()->second->getCubeSide();
    }
    else
    {
        return VGraphicsNode::DEFAULT_CUBE_SIDE;
    }
}

std::shared_ptr<const std::vector<std::pair<uint, QVector3D> > >
    VGraphicsLayer::getNodesCoords(const SbViewportRegion & viewPortRegion, const SoPath * path) const
{
    if (path->getTail() != this)
    {
        return std::shared_ptr<const std::vector<std::pair<uint, QVector3D> > >() ;
    }
    SoGetMatrixAction * getmatrixaction = new SoGetMatrixAction(viewPortRegion);
    std::shared_ptr<std::vector<std::pair<uint, QVector3D> > > coordsVect(
                new std::vector<std::pair<uint, QVector3D> >);
    coordsVect->reserve(m_graphicsNodes.size());
    for (auto &node: m_graphicsNodes)
    {
        SoPath * translationPath = path->copy();
        translationPath->ref();
        translationPath->append(node.first);
        translationPath->append(node.second->getTranslationId());
        getmatrixaction->apply(translationPath);
        SbMatrix &transformation = getmatrixaction->getMatrix();
        SbVec3f translation;
        SbRotation rotation;
        SbVec3f scalevector;
        SbRotation scaleorientation;
        transformation.getTransform(translation, rotation, scalevector, scaleorientation);
        float x, y, z;
        translation.getValue(x, y, z);
        coordsVect->push_back(std::make_pair(node.second->getSimId(), QVector3D(x, y, z)));
        translationPath->unref();
    }
    return coordsVect;
}
