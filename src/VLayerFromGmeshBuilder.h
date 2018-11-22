/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERFROMGMESHBUILDER_H
#define _VLAYERFROMGMESHBUILDER_H

#include "VLayerFromFileBuilder.h"


class VLayerFromGmeshBuilder: public VLayerFromFileBuilder {
public: 
    
/**
 * @param filename
 * @param param VSimParamPtr
 */
void VLayerFromGmeshBuilder(QString filename, void param VSimParamPtr);
    
void importNodes();
    
void importConnections();
};

#endif //_VLAYERFROMGMESHBUILDER_H