/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERABSTRACTBUILDER_H
#define _VLAYERABSTRACTBUILDER_H

#include "../sim_elements/VLayer.h"

class VLayerAbstractBuilder
{
public:     
    enum VUnit {MM, M};
    static const float MM_IN_M;

    VLayerAbstractBuilder(const VCloth &material);
    VLayerAbstractBuilder(const VCloth &material, const QString & layerName);
    virtual ~VLayerAbstractBuilder();
    VLayerAbstractBuilder(const VLayerAbstractBuilder& ) = delete;
    VLayerAbstractBuilder& operator= (const VLayerAbstractBuilder& ) = delete;
    virtual const VLayer::ptr &build() = 0;

    void setLayerId(uint id);
    void setNodeStartId(uint id);
    void setTriangleStartId(uint id);

    uint getLayerId() const;
    uint getNodeMaxId() const;
    uint getTriangleMaxId() const;

    void setLayerName(const QString &layerName);
    const QString & getLayerName() const;

protected:
    VLayer::ptr m_pLayer;
    const VCloth::ptr m_pMaterial;
    uint m_layerId;
    uint m_nodeStartId;
    uint m_triangleStartId;
    uint m_nodeMaxId;
    uint m_triangleMaxId;
    QString m_layerName;
};

#endif //_VLAYERABSTRACTBUILDER_H
