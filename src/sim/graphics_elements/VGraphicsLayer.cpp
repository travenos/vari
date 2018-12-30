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

#include "VGraphicsLayer.h"

#include "VGraphicsNode.h"
#include "VGraphicsTriangle.h"

VGraphicsLayer::VGraphicsLayer(const VLayer::const_ptr &simLayer, uint number) :
    m_number(number),
    m_visible(false),
    m_pTransform(new SoTransform)
{
    addChild(m_pTransform);
    m_transformId = findChild(m_pTransform);
    m_pTransform->ref();
    const VSimNode::map_ptr &simNodes = simLayer->getNodes();
    reserveNodes(simNodes->size());
    for (auto &simNode : *simNodes)
        addNode(simNode.second);
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

inline void VGraphicsLayer::addNode(const VSimNode::const_ptr &simNode)
{
    VGraphicsNode * node = new VGraphicsNode(simNode);
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

uint VGraphicsLayer::getNumber() const
{
    return m_number;
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
