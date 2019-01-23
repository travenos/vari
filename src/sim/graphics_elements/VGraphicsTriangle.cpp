/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VGraphicsTriangle.h"
#include <Inventor/nodes/SoTriangleStripSet.h>
#include <Inventor/nodes/SoCoordinate3.h>

/**
 * VGraphicsTriangle implementation
 */


VGraphicsTriangle::VGraphicsTriangle(const VSimTriangle::const_ptr &simTriangle):
    VGraphicsElement(std::dynamic_pointer_cast<const VSimElement>(simTriangle)),
    m_pSimTriangle(simTriangle),
    m_pTriangleCoordinates(new SoCoordinate3),
    m_pStrip(new SoTriangleStripSet)
{
    updatePosition();
    addChild(m_pTriangleCoordinates);
    m_pTriangleCoordinates->ref();
    m_pStrip->numVertices.setValue(m_pSimTriangle->VERTICES_NUMBER);
    addChild(m_pStrip);
    m_pStrip->ref();
}

VGraphicsTriangle::~VGraphicsTriangle()
{
    m_pTriangleCoordinates->unref();
    m_pStrip->unref();
}

void VGraphicsTriangle::updatePosition() 
{
    QVector3D vert[VSimTriangle::VERTICES_NUMBER];
    m_pSimTriangle->getVertices(vert);
    for (uint i = 0; i < VSimTriangle::VERTICES_NUMBER; ++i)
        m_pTriangleCoordinates->point.set1Value(i, vert[i].x(), vert[i].y(), vert[i].z());
}
