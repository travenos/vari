/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERABSTRACTBUILDER_H
#define _VLAYERABSTRACTBUILDER_H

class VLayerAbstractBuilder {
public: 
    
virtual VLayerPtr build() = 0;
protected: 
    
virtual VLayerPtr createLayer() = 0;
};

#endif //_VLAYERABSTRACTBUILDER_H