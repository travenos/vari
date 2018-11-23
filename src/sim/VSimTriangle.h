/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMTRIANGLE_H
#define _VSIMTRIANGLE_H

#include "VSimElement.h"
#include "VSimNode.h"


class VSimTriangle: protected VSimElement {
public: 
    typedef std::shared_ptr<VSimTriangle> ptr;
    typedef std::shared_ptr<const VSimTriangle> const_ptr;
    
/**
 * @param p_material
 * @param p_node0
 * @param p_node1
 * @param p_node2
 */
VSimTriangle(VMaterial::const_ptr p_material,
             VSimNode::const_ptr p_node0,
             VSimNode::const_ptr p_node1,
             VSimNode::const_ptr p_node2);
    
std::vector<QVector3D> getVertices() const noexcept;
void updateColor() noexcept;
QColor getColor() const noexcept;
private:
    const VSimNode::const_ptr m_pNodes[3];
    QColor m_color;

};

#endif //_VSIMTRIANGLE_H
