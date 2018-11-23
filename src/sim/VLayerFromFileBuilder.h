/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERFROMFILEBUILDER_H
#define _VLAYERFROMFILEBUILDER_H

#include <map>
#include "VLayerAbstractBuilder.h"

class VLayerFromFileBuilder: protected VLayerAbstractBuilder {
public: 
    
/**
 * @param filename
 * @param param VSimParam::ptr
 */
VLayerFromFileBuilder(const QString &filename, VSimParam::ptr simParam);
VLayer::ptr build() noexcept;

protected: 
    std::map<int, VNode::ptr> m_nodes;
    std::map<int, VTriangle::ptr> m_triangles;
    VSimParam::ptr m_param;
    
void addShapePart() noexcept;
void createConnections() noexcept;
virtual void importNodes() = 0;    
virtual void importConnections() = 0;
private:
VLayer::ptr createLayer() noexcept;
};

#endif //_VLAYERFROMFILEBUILDER_H
