/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VGraphicsTriangle.h"

/**
 * VGraphicsTriangle implementation
 */


/**
 * @param simTriangle
 */
VGraphicsTriangle::VGraphicsTriangle(const VSimTriangle::const_ptr &simTriangle):
VGraphicsElement(std::dynamic_pointer_cast<const VSimElement>(simTriangle)),
  m_pSimTriangle(simTriangle),
  m_pTriangleCoordinates(new SoCoordinate3)
{
    updatePosition();
    addChild(m_pTriangleCoordinates);
}

void VGraphicsTriangle::updatePosition() noexcept
{
    QVector3D vert[m_pSimTriangle->VERTICES_NUMBER];
    m_pSimTriangle->getVertices(vert);
    for (unsigned int i = 0; i < m_pSimTriangle->VERTICES_NUMBER; ++i)
        m_pTriangleCoordinates->point.set1Value(i, vert[i].x(), vert[i].y(), vert[i].z());
}
