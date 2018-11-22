/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VLayerAbstractBuilder.h"

/**
 * VLayerAbstractBuilder implementation
 */


/**
 * @return VLayerPtr
 */
virtual VLayerPtr VLayerAbstractBuilder::build() {
    return null;
}

/**
 * @return VLayerPtr
 */
virtual VLayerPtr VLayerAbstractBuilder::createLayer() {
    return null;
}