/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERFROMANSYSBUILDER_H
#define _VLAYERFROMANSYSBUILDER_H

#include "VLayerFromFileBuilder.h"
#include <QString>

class VLayerFromAnsysBuilder: protected VLayerFromFileBuilder {
public: 
    
/**
 * @param filename
 * @param param VSimParam::ptr
 */
VLayerFromAnsysBuilder(const QString &filename, VSimParam::ptr simParam);
void importNodes() noexcept;
void importConnections() noexcept;
};

#endif //_VLAYERFROMANSYSBUILDER_H
