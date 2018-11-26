/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VGraphicsNode.h"
#include "VSimElement.h"

/**
 * VGraphicsNode implementation
 */


/**
 * @param simNode
 */
VGraphicsNode::VGraphicsNode(VSimNode::const_ptr simNode):
    VGraphicsElement(std::dynamic_pointer_cast<const VSimElement>(simNode))
{

}

void VGraphicsNode::updatePosition() noexcept {

}
