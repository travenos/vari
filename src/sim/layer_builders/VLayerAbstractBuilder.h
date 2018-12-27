/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERABSTRACTBUILDER_H
#define _VLAYERABSTRACTBUILDER_H

#include "../VLayer.h"

class VLayerAbstractBuilder
{
public:     
    enum VUnit {MM, M};
    static const float MM_IN_M;

    VLayerAbstractBuilder(const VCloth &material);
    virtual ~VLayerAbstractBuilder();
    VLayerAbstractBuilder(const VLayerAbstractBuilder& ) = delete;
    VLayerAbstractBuilder& operator= (const VLayerAbstractBuilder& ) = delete;
    virtual const VLayer::ptr &build() = 0;

    void setNodeStartId(uint id);
    void setTriangleStartId(uint id);

    uint getNodeMaxId() const;
    uint getTriangleMaxId() const;

protected:
    VLayer::ptr m_pLayer;
    const VCloth::ptr m_pMaterial;
    uint m_nodeStartId;
    uint m_triangleStartId;
    uint m_nodeMaxId;
    uint m_triangleMaxId;
};

#endif //_VLAYERABSTRACTBUILDER_H
