/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSTRIANGLE_H
#define _VGRAPHICSTRIANGLE_H

#include "VGraphicsElement.h"


class VGraphicsTriangle: public VGraphicsElement {
public: 
    
/**
 * @param simTriangle
 */
void VGraphicsTriangle(VTrianglePtr simTriangle);
    
void updatePosition();
private: 
    SoCoordinate3* m_pTriangleCoordinates;
    VTrianglePtr m_pSimTriangle;
};

#endif //_VGRAPHICSTRIANGLE_H