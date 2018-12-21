/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERFROMGMESHBUILDER_H
#define _VLAYERFROMGMESHBUILDER_H

#include "VLayerFromFileBuilder.h"

class VLayerFromGmeshBuilder: public VLayerFromFileBuilder
{
    public:

    VLayerFromGmeshBuilder(const QString &filename,
                           const VCloth &material,
                           VUnit units=M);
    bool importNodes() override;
    bool importConnections() override;
};

#endif //_VLAYERFROMGMESHBUILDER_H
