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
                           const VSimulationParametres::const_ptr &p_simParam,
                           VUnit units=M);
    bool importNodes() override;
    bool importConnections() override;
};

#endif //_VLAYERFROMGMESHBUILDER_H
