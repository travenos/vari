/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSTRIANGLE_H
#define _VGRAPHICSTRIANGLE_H

#include <Inventor/nodes/SoCoordinate3.h>
#include "VGraphicsElement.h"
#include "VSimTriangle.h"

class VGraphicsTriangle: protected VGraphicsElement {
public: 
    
/**
 * @param simTriangle
 */
VGraphicsTriangle(VTriangle::ptr simTriangle);
void updatePosition() noexcept;
private: 
    SoCoordinate3* m_::ptriangleCoordinates;
    VTriangle::ptr m_pSimTriangle;
};

#endif //_VGRAPHICSTRIANGLE_H
