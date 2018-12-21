/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERFROMANSYSBUILDER_H
#define _VLAYERFROMANSYSBUILDER_H

#include "VLayerFromFileBuilder.h"

class VLayerFromAnsysBuilder: public VLayerFromFileBuilder
{
public: 
        
    VLayerFromAnsysBuilder(const QString &filename,
                           const VCloth &material,
                           VUnit units=M);
    bool importNodes() override;
    bool importConnections() override;
};

#endif //_VLAYERFROMANSYSBUILDER_H
