/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERFROMLAYERBUILDER_H
#define _VLAYERFROMLAYERBUILDER_H

#include "VLayerAbstractBuilder.h"

class VLayerFromLayerBuilder: public VLayerAbstractBuilder
{
public:
    VLayerFromLayerBuilder(const VLayer::const_ptr &sourceLayerPtr);
    
    const VLayer::ptr &build() override;
private:
    void createNodes();
    void createTriangles();
    const VLayer::const_ptr m_pSourceLayer;
    VSimNode::map_ptr m_pNodes;
    VSimTriangle::list_ptr m_pTriangles;
};

#endif //_VLAYERFROMLAYERBUILDER_H
