/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERFROMANSYSBUILDER_H
#define _VLAYERFROMANSYSBUILDER_H

#include "VLayerFromFileBuilder.h"
#include <QString>

class VLayerFromAnsysBuilder: public VLayerFromFileBuilder {
public: 
    
/**
 * @param filename const QString
 * @param param VSimulationParametres::const_ptr
 */
VLayerFromAnsysBuilder(const QString &filename,
                       const VSimulationParametres::const_ptr &simParam);
void importNodes() noexcept;
void importConnections() noexcept;
};

#endif //_VLAYERFROMANSYSBUILDER_H
