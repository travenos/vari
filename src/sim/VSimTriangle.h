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
    typedef std::shared_ptr<std::vector<VSimTriangle::ptr> > vector_ptr;
    typedef std::shared_ptr<const std::vector<VSimTriangle::ptr> > const_vector_ptr;

    static const unsigned int VERTICES_NUMBER = 3;
/**
 * @param p_material
 * @param p_node0
 * @param p_node1
 * @param p_node2
 */
VSimTriangle(VCloth::const_ptr p_material, VSimulationParametres::const_ptr p_param,
             VSimNode::const_ptr p_node0,
             VSimNode::const_ptr p_node1,
             VSimNode::const_ptr p_node2);
    
void getVertices(QVector3D vertices[VERTICES_NUMBER]) const noexcept;
void updateColor() noexcept;
const QColor& getColor() const noexcept;
double getAveragePressure() const noexcept;
void reset() noexcept override;
private:
    const VSimNode::const_weak_ptr m_pNodes[VERTICES_NUMBER];
    QColor m_color;

};

#endif //_VSIMTRIANGLE_H
