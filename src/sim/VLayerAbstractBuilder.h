/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERABSTRACTBUILDER_H
#define _VLAYERABSTRACTBUILDER_H

#include "VLayer.h"

class VLayerAbstractBuilder
{
public:     
    enum VUnit {MM, M};
    static const float MM_IN_M;

    VLayerAbstractBuilder(const VCloth &material,
                          const VSimulationParametres::const_ptr &p_simParam);
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
    const VSimulationParametres::const_ptr m_pParam;
    uint m_nodeStartId;
    uint m_triangleStartId;
    uint m_NodeMaxId;
    uint m_TriangleMaxId;
};

#endif //_VLAYERABSTRACTBUILDER_H
