/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERFROMANSYSBUILDER_H
#define _VLAYERFROMANSYSBUILDER_H

#include "VLayerFromFileBuilder.h"


class VLayerFromAnsysBuilder: public VLayerFromFileBuilder {
public: 
    
/**
 * @param filename
 * @param param VSimParamPtr
 */
void VLayerFromAnsysBuilder(QString filename, void param VSimParamPtr);
    
void importNodes();
    
void importConnections();
};

#endif //_VLAYERFROMANSYSBUILDER_H