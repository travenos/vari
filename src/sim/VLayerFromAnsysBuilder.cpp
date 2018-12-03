/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VLayerFromAnsysBuilder.h"

/**
 * VLayerFromAnsysBuilder implementation
 */


/**
 * @param filename const QString
 * @param param VSimulationParametres::const_ptr
 */
VLayerFromAnsysBuilder::VLayerFromAnsysBuilder(const QString &filename,
                                               const VSimulationParametres::const_ptr &simParam):
    VLayerFromFileBuilder(filename, simParam)
{

}

void VLayerFromAnsysBuilder::importNodes()  noexcept {

}

void VLayerFromAnsysBuilder::importConnections()  noexcept {

}
