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
 * @param param VSimParam::const_ptr
 */
VLayerFromGmeshBuilder(const QString &filename,
                       const VCloth &material,
                       const VSimulationParametres::const_ptr &p_simParam);
bool importNodes() noexcept;
bool importConnections() noexcept;
};

#endif //_VLAYERFROMGMESHBUILDER_H