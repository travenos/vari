/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERFROMFILEBUILDER_H
#define _VLAYERFROMFILEBUILDER_H

#include <map>
#include "VLayerAbstractBuilder.h"
#include "VSimNode.h"
#include "VSimTriangle.h"
#include "VSimulationParametres.h"

class VLayerFromFileBuilder: protected VLayerAbstractBuilder {
public: 
    
/**
 * @param filename
 * @param param VSimParam::ptr
 */
VLayerFromFileBuilder(const QString &filename, VSimulationParametres::const_ptr simParam);
VLayer::ptr build() noexcept;

protected: 
    std::map<int, VSimNode::ptr> m_nodes;
    std::map<int, VSimTriangle::ptr> m_triangles;
    VSimulationParametres::ptr m_param;
    
void addShapePart() noexcept;
void createConnections() noexcept;
virtual void importNodes() = 0;    
virtual void importConnections() = 0;
private:
VLayer::ptr createLayer() noexcept;
};

#endif //_VLAYERFROMFILEBUILDER_H
