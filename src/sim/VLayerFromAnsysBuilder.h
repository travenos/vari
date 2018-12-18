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
                           const VSimulationParametres::const_ptr &p_simParam,
                           VUnit units=M);
    bool importNodes() override;
    bool importConnections() override;
};

#endif //_VLAYERFROMANSYSBUILDER_H
