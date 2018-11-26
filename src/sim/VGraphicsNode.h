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
VGraphicsNode(VSimNode::const_ptr simNode);
void updatePosition() noexcept;
private: 
    SoTranslation* m_pTranslation;
    SoCube* m_pCube;
    VSimNode::const_ptr m_pSimNode;
};

#endif //_VGRAPHICSNODE_H
