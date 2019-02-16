/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VLAYER_H
#define _VLAYER_H

#include <vector>
#include <memory>
#include <QColor>

#include "VSimNode.h"
#include "VSimTriangle.h"
#include "VNodesVolume.h"

class VLayer
{
public: 
    typedef std::shared_ptr<VLayer> ptr;
    typedef std::shared_ptr<const VLayer> const_ptr;

    VLayer() = delete;
    VLayer(const VSimNode::map_ptr &nodes, const VSimTriangle::list_ptr &triangles,
           const VCloth::ptr &material, bool createVolume=true);
    virtual ~VLayer();
    VLayer(const VLayer& ) = delete;
    VLayer& operator= (const VLayer& ) = delete;
    VLayer(VLayer&&) = delete;
    VLayer& operator= (VLayer&& ) = delete;

    void resetNodesVolume();
    bool isNodesVolumeValid() const;
    void getSize(QVector3D &size) const;
    void getConstrains(QVector3D &min, QVector3D &max) const;
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
    void transformate(const std::shared_ptr<const std::vector<std::pair<uint, QVector3D> > >
                        &nodesCoords);

    void connectWith(const VLayer::ptr &otherLayer);
    void disconnect();

    static const float SEARCH_ZONE_PART;

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

    VNodesVolume m_nodesVolume;
};

#endif //_VLAYER_H
