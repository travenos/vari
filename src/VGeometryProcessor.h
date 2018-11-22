/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGEOMETRYPROCESSOR_H
#define _VGEOMETRYPROCESSOR_H

class VGeometryProcessor {
public: 
    
/**
 * @param polygon
 */
static VLayerPtr createFromPolygon(VPolygon polygon);
    
/**
 * @param polygon
 */
static void cutByPolygon(VPolygon polygon);
    
/**
 * @param matrix
 */
static void transform(VMatrix& matrix);
};

#endif //_VGEOMETRYPROCESSOR_H