/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERFROMFILEBUILDER_H
#define _VLAYERFROMFILEBUILDER_H

#include "VLayerAbstractBuilder.h"


class VLayerFromFileBuilder: public VLayerAbstractBuilder {
public: 
    
/**
 * @param filename
 * @param param VSimParamPtr
 */
void VLayerFromFileBuilder(QString filename, void param VSimParamPtr);
    
VLayerPtr build();
protected: 
    std::map<int, VNodePtr> m_nodes;
    std::map<int, VTrianglePtr> m_triangles;
    VSimParamPtr m_param;
    
void addShapePart();
    
void createConnections();
    
virtual void importNodes() = 0;
    
virtual void importConnections() = 0;
private: 
    
VLayerPtr createLayer();
};

#endif //_VLAYERFROMFILEBUILDER_H