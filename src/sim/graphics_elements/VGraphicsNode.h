/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSNODE_H
#define _VGRAPHICSNODE_H

#include "VGraphicsElement.h"
#include "../sim_elements/VSimNode.h"

class SoTranslation;
class SoCube;
class SbViewportRegion;

class VGraphicsNode: public VGraphicsElement {
public: 
    static const float DEFAULT_CUBE_SIDE;
    
    VGraphicsNode(const VSimNode::const_ptr &simNode);
    virtual ~VGraphicsNode();
    void updatePosition() ;
    void setCubeSide(float side) ;
    int getTranslationId() const;
private:
    VSimNode::const_ptr m_pSimNode;
    SoTranslation* m_pTranslation;
    SoCube* m_pCube;
    int m_pTranslationId;
};

#endif //_VGRAPHICSNODE_H
