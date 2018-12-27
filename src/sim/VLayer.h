/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYER_H
#define _VLAYER_H

#include <vector>
#include <memory>
#include <QColor>
#include "sim_elements/VSimNode.h"
#include "sim_elements/VSimTriangle.h"

class VLayer
{
public: 
    typedef std::shared_ptr<VLayer> ptr;
    typedef std::shared_ptr<const VLayer> const_ptr;

    /**
     * @param nodes
     * @param triangles
     * @param material
     */
    VLayer() = delete;
    VLayer(const VSimNode::map_ptr &nodes, const VSimTriangle::list_ptr &triangles,
           const VCloth::ptr &material);
    virtual ~VLayer();
    VLayer(const VLayer& ) = delete; //TODO implement theese methods
    VLayer& operator= (const VLayer& ) = delete; //TODO implement theese methods
    //TODO Also implement move and rvalue
    /**
     * @param visible
     */
    void setVisible(bool visible) ;
    bool isVisible() const ;
    void markActive(bool active) ;
    bool isActive() const ;
    size_t getNodesNumber() const ;
    size_t getTrianglesNumber() const ;
    const VSimNode::map_ptr &getNodes() const;
    const VSimTriangle::list_ptr &getTriangles() const ;
    void setMateial(const VCloth &material) ;
    void reset() ;
    VCloth::const_ptr getMaterial() const ;

    void setMinMaxIds(uint nodeMinId, uint nodeMaxId, uint tiangleMinId, uint triangleMaxId);
    uint getNodeMinId() const;
    uint getNodeMaxId() const;
    uint getTriangleMinId() const;
    uint getTriangleMaxId() const;
    void cut(const std::shared_ptr<const std::vector<uint> > &nodesIds);

private: 
    void p_setVisible(bool visible) ;

    VSimNode::map_ptr m_pNodes;
    VSimTriangle::list_ptr m_pTriangles;
    const VCloth::ptr m_pMaterial;
    bool m_visible;
    bool m_wasVisible;
    bool m_active;

    uint m_nodeMinId;
    uint m_nodeMaxId;
    uint m_triangleMinId;
    uint m_triangleMaxId;
};

#endif //_VLAYER_H
