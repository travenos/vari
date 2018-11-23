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
 * @param param VSimParam::ptr
 */
VLayerFromGmeshBuilder(const QString &filename, VSimParam::ptr simParam);
void importNodes() noexcept;
void importConnections() noexcept;
};

#endif //_VLAYERFROMGMESHBUILDER_H
