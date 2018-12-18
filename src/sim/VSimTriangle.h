/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMTRIANGLE_H
#define _VSIMTRIANGLE_H

#include <vector>
#include "VSimNode.h"

class VSimTriangle: public VSimElement
{
public: 
    typedef std::shared_ptr<VSimTriangle> ptr;
    typedef std::shared_ptr<const VSimTriangle> const_ptr;
    typedef std::shared_ptr<std::vector<VSimTriangle::ptr> > vector_ptr;
    typedef std::shared_ptr<const std::vector<VSimTriangle::ptr> > const_vector_ptr;

    static const uint VERTICES_NUMBER = 3;
/**
 * @param p_material
 * @param p_node0
 * @param p_node1
 * @param p_node2
 */
VSimTriangle(uint id, const VCloth::const_ptr &p_material,
             const VSimulationParametres::const_ptr &p_param,
             const VSimNode::const_ptr &p_node0,
             const VSimNode::const_ptr &p_node1,
             const VSimNode::const_ptr &p_node2);
    
void getVertices(QVector3D vertices[VERTICES_NUMBER]) const ;
void updateColor() ;
const QColor& getColor() const override;
double getPressure() const override;
double getFilledPart() const override;
void reset()  override;

bool isInjection() const override;
bool isVacuum() const override;
bool isNormal() const override;

private:
    QColor m_color;
    const std::vector<const VSimNode *>m_pNodes;

    bool isRole(VSimNode::VNodeRole role) const;
};

#endif //_VSIMTRIANGLE_H
