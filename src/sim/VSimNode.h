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
    typedef std::weak_ptr<VSimNode> weak_ptr;
    typedef std::weak_ptr<const VSimNode> const_weak_ptr;
    typedef std::shared_ptr<std::vector<VSimNode::ptr> > vector_ptr;
    typedef std::shared_ptr<const std::vector<VSimNode::ptr> > const_vector_ptr;

    enum VLayerSequence { PREVIOUS, CURRENT, NEXT };
    enum VNodeRole { NORMAL, INJECTION, VACUUM };
/**
 * @param pos
 * @param p_material
 * @param p_param
 */
VSimNode(QVector3D& pos, VCloth::const_ptr p_material, VSimulationParametres::const_ptr p_param);
    
/**
 * @param role
 */
void setRole(VNodeRole role) noexcept;
VNodeRole getRole() const noexcept;
void calculate() noexcept;
bool commit() noexcept;
double getPressure() const noexcept;
/**
 * @param neighbour
 * @param layer
 */
void addNeighbourMutually(VSimNode::ptr node, VLayerSequence layer=CURRENT) noexcept;
void addNeighbour(VSimNode::const_ptr node, VLayerSequence layer=CURRENT) noexcept;
void clearAllNeighbours() noexcept;
/**
 * @param layer
 */
void clearNeighbours(VLayerSequence layer) noexcept;

double getDistance(VSimNode::const_ptr node) const noexcept;
const QVector3D &getPosition() const noexcept;
void reset() noexcept override;
void getNeighbours(std::vector<const_ptr> &neighbours) const noexcept;
int getNeighboursNumber() const noexcept;
double getCavityHeight() const noexcept;
double getPorosity() const noexcept;
double getPermeability() const noexcept;

private: 

struct VLayeredNeighbours
{
    std::multimap<double, VSimNode::const_weak_ptr> previousLayerNeighbours;
    std::multimap<double, VSimNode::const_weak_ptr> currentLayerNeighbours;
    std::multimap<double, VSimNode::const_weak_ptr> nextLayerNeighbours;
};

    VNodeRole m_role;
    double m_currentPressure;
    double m_newPressure;
    VLayeredNeighbours m_neighbours;
    std::vector<std::weak_ptr<VSimTriangle>> m_pTriangles;
    QVector3D m_position;
    int m_neighboursNumber;
};

#endif //_VSIMNODE_H
