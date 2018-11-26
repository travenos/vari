/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMTRIANGLE_H
#define _VSIMTRIANGLE_H

#include <vector>
#include "VSimNode.h"

class VSimTriangle: public VSimElement {
public: 
    typedef std::shared_ptr<VSimTriangle> ptr;
    typedef std::shared_ptr<const VSimTriangle> const_ptr;
    typedef std::weak_ptr<VSimTriangle> weak_ptr;
    typedef std::weak_ptr<const VSimTriangle> const_weak_ptr;
    
/**
 * @param p_material
 * @param p_node0
 * @param p_node1
 * @param p_node2
 */
VSimTriangle(VCloth::const_ptr p_material,
             VSimNode::const_ptr p_node0,
             VSimNode::const_ptr p_node1,
             VSimNode::const_ptr p_node2);
    
std::vector<QVector3D> getVertices() const noexcept;
void updateColor() noexcept;
const QColor& getColor() const noexcept;
private:
    const VSimNode::const_weak_ptr m_pNodes[3];
    QColor m_color;

};

#endif //_VSIMTRIANGLE_H
