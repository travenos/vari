/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoBaseColor.h>
#include "VGraphicsNode.h"
#include "VSimElement.h"

/**
 * VGraphicsNode implementation
 */

const float VGraphicsNode::DEFAULT_CUBE_SIDE = 15.0;

/**
 * @param simNode
 */
VGraphicsNode::VGraphicsNode(const VSimNode::const_ptr &simNode):
    VGraphicsElement(std::dynamic_pointer_cast<const VSimElement>(simNode)),
    m_pSimNode(simNode),
    m_pTranslation(new SoTranslation),
    m_pCube(new SoCube)
{
    updatePosition();
    addChild(m_pTranslation);

    setCubeSide(DEFAULT_CUBE_SIDE);
    addChild(m_pCube);
}

void VGraphicsNode::updatePosition() noexcept
{
    const QVector3D& pos = m_pSimNode->getPosition();
    m_pTranslation->translation.setValue(pos.x(), pos.y(), pos.z());
}

void VGraphicsNode::setCubeSide(float side) noexcept
{
    m_pCube->width = side;
    m_pCube->height = side;
    m_pCube->depth = side;
}
