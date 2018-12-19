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
#include <atomic>

#include "VSimElement.h"

class VSimTriangle;

class VSimNode: public VSimElement {
public: 
    typedef std::shared_ptr<VSimNode> ptr;
    typedef std::shared_ptr<const VSimNode> const_ptr;
    typedef std::shared_ptr<std::vector<VSimNode::ptr> > vector_ptr;
    typedef std::shared_ptr<const std::vector<VSimNode::ptr> > const_vector_ptr;

    enum VLayerSequence { PREVIOUS=0, CURRENT, NEXT };
    enum VNodeRole { NORMAL, INJECTION, VACUUM };
/**
 * @param pos
 * @param p_material
 * @param p_param
 */
VSimNode(uint id, const QVector3D &pos,
         const VCloth::const_ptr &p_material,
         const VSimulationParametres::const_ptr &p_param);
    
/**
 * @param role
 */
void setRole(VNodeRole role) ;
VNodeRole getRole() const ;
void calculate() ;
void commit(bool *madeChanges=nullptr, bool *isFull=nullptr) ;
double getPressure() const override;
double getFilledPart() const override;
double getNewPressure() const;
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
double getDistance(const QVector3D& point) const;
const QVector3D &getPosition() const ;
void reset()  override;
void getNeighbours(std::vector<const VSimNode *> &neighbours) const ;
void getNeighbours(std::vector<const VSimNode *> &neighbours, VLayerSequence layer) const ;
void getNeighboursId(std::vector<uint> &neighbourId) const ;
void getNeighboursId(std::vector<uint> &neighbourId, VLayerSequence layer) const;
size_t getNeighboursNumber() const ;
size_t getNeighboursNumber(VLayerSequence layer) const ;
double getCavityHeight() const ;
double getPorosity() const ;
double getPermeability() const ;

bool isInjection() const override;
bool isVacuum() const override;
bool isNormal() const override;

private: 

//struct VLayeredNeighbours
//{
//    std::multimap<double, const VSimNode*> previousLayerNeighbours;
//    std::multimap<double, const VSimNode*> currentLayerNeighbours;
//    std::multimap<double, const VSimNode*> nextLayerNeighbours;
//    //TODO: Implement size(), clear(), simple iterator
//};
    typedef std::multimap<double, const VSimNode*> VLayeredNeighbours[3];

    VNodeRole m_role;
    std::atomic<double> m_currentPressure;
    std::atomic<double> m_newPressure;
    VLayeredNeighbours m_neighbours;
    QVector3D m_position;
    size_t m_neighboursNumber;
};

#endif //_VSIMNODE_H
