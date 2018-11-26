/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VGraphicsTriangle.h"

/**
 * VGraphicsTriangle implementation
 */


/**
 * @param simTriangle
 */
VGraphicsTriangle::VGraphicsTriangle(VSimTriangle::const_ptr simTriangle):
VGraphicsElement(std::dynamic_pointer_cast<const VSimElement>(simTriangle))
{

}

void VGraphicsTriangle::updatePosition() noexcept {

}
