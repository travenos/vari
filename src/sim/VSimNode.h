/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMNODE_H
#define _VSIMNODE_H

#include <QVector3D>
#include <vector>
#include <map>
#include <memory>
#include "VSimElement.h"

class VSimTriangle;

class VSimNode: public VSimElement {
public: 
    typedef std::shared_ptr<VSimNode> ptr;
    typedef std::shared_ptr<const VSimNode> const_ptr;
    typedef std::shared_ptr<std::vector<VSimNode::ptr> > vector_ptr;
    typedef std::shared_ptr<const std::vector<VSimNode::ptr> > const_vector_ptr;

    enum VLayerSequence { PREVIOUS, CURRENT, NEXT };
    enum VNodeRole { NORMAL, INJECTION, VACUUM };
/**
 * @param pos
 * @param p_material
 * @param p_param
 */
VSimNode(const QVector3D &pos,
         const VCloth::const_ptr &p_material,
         const VSimulationParametres::const_ptr &p_param);
    
/**
 * @param role
 */
void setRole(VNodeRole role) ;
VNodeRole getRole() const ;
void calculate() ;
bool commit() ;
double getPressure() const ;
/**
 * @param neighbour
 * @param layer
 */
void addNeighbourMutually(VSimNode *node, VLayerSequence layer=CURRENT) ;
void addNeighbour(const VSimNode *node, VLayerSequence layer=CURRENT) ;
void clearAllNeighbours() ;
/**
 * @param layer
 */
void clearNeighbours(VLayerSequence layer) ;

double getDistance(const VSimNode* node) const ;
const QVector3D &getPosition() const ;
void reset()  override;
void getNeighbours(std::vector<const VSimNode *> &neighbours) const ;
size_t getNeighboursNumber() const ;
double getCavityHeight() const ;
double getPorosity() const ;
double getPermeability() const ;

private: 

struct VLayeredNeighbours
{
    std::multimap<double, const VSimNode*> previousLayerNeighbours;
    std::multimap<double, const VSimNode*> currentLayerNeighbours;
    std::multimap<double, const VSimNode*> nextLayerNeighbours;
};

    VNodeRole m_role;
    double m_currentPressure;
    double m_newPressure;
    VLayeredNeighbours m_neighbours;
    std::vector<const VSimTriangle*> m_pTriangles; // TODO Is it really necessary?
    QVector3D m_position;
    size_t m_neighboursNumber;
};

#endif //_VSIMNODE_H
