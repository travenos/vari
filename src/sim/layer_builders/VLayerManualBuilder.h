/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERMANUALBUILDER_H
#define _VLAYERMANUALBUILDER_H

#include <list>

#include "VLayerAbstractBuilder.h"

typedef std::list< std::pair<float, float> > VPolygon;

class VLayerManualBuilder: public VLayerAbstractBuilder {
public: 
    
const VLayer::ptr &build() ;
};

#endif //_VLAYERMANUALBUILDER_H
