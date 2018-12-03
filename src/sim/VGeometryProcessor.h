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
static void cutByPolygon(const VLayer::ptr &layer, const VPolygon &polygon) noexcept;
    
/**
 * @param matrix
 */
static void transform(const VLayer::ptr &layer, const VMatrix &matrix) noexcept;
};

#endif //_VGEOMETRYPROCESSOR_H
