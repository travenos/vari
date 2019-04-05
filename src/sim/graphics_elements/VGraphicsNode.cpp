/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif

#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoBaseColor.h>

#include "VGraphicsNode.h"
#include "../sim_elements/VSimElement.h"

/**
 * VGraphicsNode implementation
 */

const float VGraphicsNode::DEFAULT_CUBE_SIDE = 0; //1.5e-3f;

VGraphicsNode::VGraphicsNode(const VSimNode::const_ptr &simNode, float cubeSide):
    VGraphicsElement(std::dynamic_pointer_cast<const VSimElement>(simNode)),
    m_pSimNode(simNode),
    m_pTranslation(new SoTranslation),
    m_pCube(new SoCube)
{
    updatePosition();
    addChild(m_pTranslation);
    m_pTranslation->ref();
    m_translationId = findChild(m_pTranslation);

    setCubeSide(cubeSide);
    addChild(m_pCube);
    m_pCube->ref();
}

VGraphicsNode::~VGraphicsNode()
{
    m_pTranslation->unref();
    m_pCube->unref();
}

void VGraphicsNode::updatePosition() 
{
    const QVector3D& pos = m_pSimNode->getPosition();
    m_pTranslation->translation.setValue(pos.x(), pos.y(), pos.z());
}

void VGraphicsNode::setCubeSide(float side) 
{
    m_pCube->width = side;
    m_pCube->height = side;
    m_pCube->depth = side;
}

float VGraphicsNode::getCubeSide() const
{
    return m_pCube->width.getValue();
}

int VGraphicsNode::getTranslationId() const
{
    return m_translationId;
}
