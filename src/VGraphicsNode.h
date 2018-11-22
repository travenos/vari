/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSNODE_H
#define _VGRAPHICSNODE_H

#include "VGraphicsElement.h"


class VGraphicsNode: public VGraphicsElement {
public: 
    
/**
 * @param simNode
 */
void VGraphicsNode(VNodePtr simNode);
    
void updatePosition();
private: 
    SoTranslation* m_pTranslation;
    SoCube* m_pCube;
    VNodePtr m_pSimNode;
};

#endif //_VGRAPHICSNODE_H