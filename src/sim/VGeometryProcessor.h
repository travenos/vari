/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGEOMETRYPROCESSOR_H
#define _VGEOMETRYPROCESSOR_H

#include "VLayer.h"

struct VMatrix {
};

struct VPolygon {
};

class VGeometryProcessor {
public: 
    
/**
 * @param polygon
 */
static VLayer::ptr createFromPolygon(VPolygon const &polygon) noexcept;
    
/**
 * @param polygon
 */
static void cutByPolygon(VLayer::ptr layer, const VPolygon &polygon) noexcept;
    
/**
 * @param matrix
 */
static void transform(VLayer::ptr layer, const VMatrix &matrix) noexcept;
};

#endif //_VGEOMETRYPROCESSOR_H
