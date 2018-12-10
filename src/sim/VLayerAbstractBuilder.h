/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERABSTRACTBUILDER_H
#define _VLAYERABSTRACTBUILDER_H

#include "VLayer.h"

class VLayerAbstractBuilder {
public:     
VLayerAbstractBuilder(const VCloth &material,
                      const VSimulationParametres::const_ptr &p_simParam);
virtual ~VLayerAbstractBuilder();
VLayerAbstractBuilder(const VLayerAbstractBuilder& ) = delete;
VLayerAbstractBuilder& operator= (const VLayerAbstractBuilder& ) = delete;
virtual const VLayer::ptr &build() = 0;
protected:
VLayer::ptr m_pLayer;
const VCloth::ptr m_pMaterial;
const VSimulationParametres::const_ptr m_pParam;
};

#endif //_VLAYERABSTRACTBUILDER_H
