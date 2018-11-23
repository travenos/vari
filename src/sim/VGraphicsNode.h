/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSNODE_H
#define _VGRAPHICSNODE_H

#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCube.h>
#include "VGraphicsElement.h"
#include "VSimNode.h"


class VGraphicsNode: protected VGraphicsElement {
public: 
    
/**
 * @param simNode
 */
VGraphicsNode(VNode::ptr simNode);
void updatePosition() noexcept;
private: 
    SoTranslation* m_::ptranslation;
    SoCube* m_pCube;
    VNode::ptr m_pSimNode;
};

#endif //_VGRAPHICSNODE_H
