/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VLayerFromGmeshBuilder.h"

/**
 * VLayerFromGmeshBuilder implementation
 */


/**
 * @param filename
 * @param param VSimParam::const_ptr
 */
VLayerFromGmeshBuilder::VLayerFromGmeshBuilder(const QString &filename, VSimulationParametres::const_ptr simParam):
VLayerFromFileBuilder(filename, simParam)
{

}

void VLayerFromGmeshBuilder::importNodes() noexcept {

}

void VLayerFromGmeshBuilder::importConnections() noexcept {

}
