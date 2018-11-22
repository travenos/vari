/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMNODE_H
#define _VSIMNODE_H

#include "VSimElement.h"
#include "VSimTriangle.h"


class VSimNode: public VSimElement {
public: 
    
/**
 * @param pos
 * @param material
 * @param param
 */
void VSimNode(QVector3D& pos, VMaterialPtr material, VSimParamPtr param);
    
/**
 * @param role
 */
void setRole(VNodeRole role);
    
VNodeRole getRole();
    
void calculate();
    
void commit();
    
double getPressure();
    
/**
 * @param neighbour
 * @param layer
 */
void addNeighbour(const VSimNode* neighbour, VLayerSequence layer);
    
void clearAllNeighbours();
    
/**
 * @param layer
 */
void clearNeighbours(VLayerSequence layer);
    
QVector3D& getPosition();
    
double getMedianDistance();
    
void reset();
private: 
    VNodeRole m_role;
    double m_currentPressure;
    double m_newPressure;
    VLayeredNeighbours m_neighbours;
    vector<VTrianglePtr> m_pTriangles;
    QVector3D m_position;
    VSimParamPtr m_param;
    vector<VSimTriangle> m_pTriangles;
};

#endif //_VSIMNODE_H