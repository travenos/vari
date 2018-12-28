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

class VGraphicsNode: public VGraphicsElement {
public: 
    static const float DEFAULT_CUBE_SIDE;
    
/**
 * @param simNode
 */
VGraphicsNode(const VSimNode::const_ptr &simNode);
virtual ~VGraphicsNode();
void updatePosition() ;
void setCubeSide(float side) ;
QVector3D getPosition() const;
private:
    VSimNode::const_ptr m_pSimNode;
    SoTranslation* m_pTranslation;
    SoCube* m_pCube;
};

#endif //_VGRAPHICSNODE_H
