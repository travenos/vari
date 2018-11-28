/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSNODE_H
#define _VGRAPHICSNODE_H

#include "VGraphicsElement.h"
#include "VSimNode.h"

class SoTranslation;
class SoCube;

class VGraphicsNode: public VGraphicsElement {
public: 
    static const float DEFAULT_CUBE_SIDE;
    
/**
 * @param simNode
 */
VGraphicsNode(VSimNode::const_ptr simNode);
void updatePosition() noexcept;
void setCubeSide(float side) noexcept;
private:
    VSimNode::const_ptr m_pSimNode;
    SoTranslation* m_pTranslation;
    SoCube* m_pCube;
};

#endif //_VGRAPHICSNODE_H
