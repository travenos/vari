/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERMANUALBUILDER_H
#define _VLAYERMANUALBUILDER_H

#include "VLayerAbstractBuilder.h"


class VLayerManualBuilder: protected VLayerAbstractBuilder {
public: 
    
VLayer::ptr build() noexcept;
};

#endif //_VLAYERMANUALBUILDER_H
