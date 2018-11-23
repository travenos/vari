/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERABSTRACTBUILDER_H
#define _VLAYERABSTRACTBUILDER_H

#include "VLayer.h"

class VLayerAbstractBuilder {
public:     
virtual VLayer::ptr build() = 0;
protected:
virtual VLayer::ptr createLayer() = 0;
};

#endif //_VLAYERABSTRACTBUILDER_H
