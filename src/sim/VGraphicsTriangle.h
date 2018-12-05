/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSTRIANGLE_H
#define _VGRAPHICSTRIANGLE_H

#include "VGraphicsElement.h"
#include "VSimTriangle.h"

class SoCoordinate3;
class SoTriangleStripSet;

class VGraphicsTriangle: public VGraphicsElement {
public: 
    
/**
 * @param simTriangle
 */
VGraphicsTriangle(const VSimTriangle::const_ptr &simTriangle);
void updatePosition() noexcept;
private: 
    VSimTriangle::const_ptr m_pSimTriangle;
    SoCoordinate3* m_pTriangleCoordinates;
    SoTriangleStripSet* m_pStrip;
};

#endif //_VGRAPHICSTRIANGLE_H
