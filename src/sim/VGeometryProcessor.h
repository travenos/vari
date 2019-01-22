/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGEOMETRYPROCESSOR_H
#define _VGEOMETRYPROCESSOR_H

#include "sim_elements/VLayer.h"

struct VMatrix {
};

struct VPolygon {
};

class VGeometryProcessor {
public: 
    
/**
 * @param polygon
 */
static VLayer::ptr createFromPolygon(VPolygon const &polygon) ;
    
/**
 * @param polygon
 */
static void cutByPolygon(const VLayer::ptr &layer, const VPolygon &polygon) ;
    
/**
 * @param matrix
 */
static void transform(const VLayer::ptr &layer, const VMatrix &matrix) ;
};

#endif //_VGEOMETRYPROCESSOR_H
