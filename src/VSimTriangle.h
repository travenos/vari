/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMTRIANGLE_H
#define _VSIMTRIANGLE_H

#include "VSimElement.h"
#include "VSimNode.h"


class VSimTriangle: public VSimElement {
public: 
    
/**
 * @param material
 * @param nodes
 */
void VSimTriangle(VMaterialPtr material, VNodePtr[] nodes);
    
std::vector<QVector3D> getVertices();
    
void updateColor();
    
QColor& getColor();
private: 
    VNodePtr[] m_pNodes;
    QColor m_color;
    VSimNode[] m_pNodes;
};

#endif //_VSIMTRIANGLE_H