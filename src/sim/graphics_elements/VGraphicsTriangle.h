/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSTRIANGLE_H
#define _VGRAPHICSTRIANGLE_H

#include "VGraphicsElement.h"
#include "../sim_elements/VSimTriangle.h"

class SoCoordinate3;
class SoTriangleStripSet;

class VGraphicsTriangle: public VGraphicsElement
{
public:     
    VGraphicsTriangle(const VSimTriangle::const_ptr &simTriangle);
    virtual ~VGraphicsTriangle();
    void updatePosition() ;
private: 
    VSimTriangle::const_ptr m_pSimTriangle;
    SoCoordinate3* m_pTriangleCoordinates;
    SoTriangleStripSet* m_pStrip;
};

#endif //_VGRAPHICSTRIANGLE_H
