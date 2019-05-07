/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMTRIANGLE_H
#define _VSIMTRIANGLE_H

#include <vector>
#include <list>
#include <unordered_map>

#include "VSimNode.h"

class VSimTriangle: public VSimElement
{
public: 
    typedef std::shared_ptr<VSimTriangle> ptr;
    typedef std::shared_ptr<const VSimTriangle> const_ptr;
    typedef std::vector<ptr> vector_t;
    typedef std::shared_ptr<vector_t> vector_ptr;
    typedef std::shared_ptr<const vector_t> const_vector_ptr;
    typedef std::unordered_map<uint, ptr> map_t;
    typedef std::shared_ptr<map_t> map_ptr;
    typedef std::shared_ptr<const map_t> const_map_ptr;
    typedef std::list<ptr> list_t;
    typedef std::shared_ptr<list_t> list_ptr;
    typedef std::shared_ptr<const list_t> const_list_ptr;

    static const uint VERTICES_NUMBER{3};

    VSimTriangle(uint id, const VCloth::const_ptr &p_material,
                 const VSimNode::const_ptr &p_node0,
                 const VSimNode::const_ptr &p_node1,
                 const VSimNode::const_ptr &p_node2);

    VSimTriangle(uint id, const VCloth::const_ptr &p_material,
                 const VSimNode::const_ptr &p_node0,
                 const VSimNode::const_ptr &p_node1,
                 const VSimNode::const_ptr &p_node2,
                 const QColor &color);

    virtual ~VSimTriangle();

    void getVertices(QVector3D vertices[VERTICES_NUMBER]) const ;
    std::vector<uint> getVerticesId() const ;
    void updateColor() ;
    const QColor& getColor() const override;
    double getPressure() const override;
    double getFilledPart() const override;
    void reset()  override;

    bool isMarkedForRemove() const override;
    bool isInjection() const override;
    bool isVacuum() const override;
    bool isNormal() const override;

private:
    QColor m_color;
    const std::vector<const VSimNode *>m_pNodes;

    bool isRole(VSimNode::VNodeRole role) const;
};

#endif //_VSIMTRIANGLE_H
