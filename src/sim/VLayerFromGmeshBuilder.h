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
                       const VSimulationParametres::const_ptr &simParam);
void importNodes() noexcept;
void importConnections() noexcept;
};

#endif //_VLAYERFROMGMESHBUILDER_H
