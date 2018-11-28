/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSTRIANGLE_H
#define _VGRAPHICSTRIANGLE_H

#include <Inventor/nodes/SoCoordinate3.h>
#include "VGraphicsElement.h"
#include "VSimTriangle.h"

class VGraphicsTriangle: public VGraphicsElement {
public: 
    
/**
 * @param simTriangle
 */
VGraphicsTriangle(VSimTriangle::const_ptr simTriangle);
void updatePosition() noexcept;
private: 
    VSimTriangle::const_ptr m_pSimTriangle;
    SoCoordinate3* m_pTriangleCoordinates;
};

#endif //_VGRAPHICSTRIANGLE_H
