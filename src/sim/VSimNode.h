/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMNODE_H
#define _VSIMNODE_H

#include <QVector3D>
#include <vector>
#include <set>
#include <memory>
#include "VSimulationParametres.h"
#include "VSimElement.h"

class VSimTriangle;

class VSimNode: public VSimElement {
public: 
    typedef std::shared_ptr<VSimNode> ptr;
    typedef std::shared_ptr<const VSimNode> const_ptr;
    typedef std::weak_ptr<VSimNode> weak_ptr;
    typedef std::weak_ptr<const VSimNode> const_weak_ptr;

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
void commit() noexcept;
double getPressure() const noexcept;
/**
 * @param neighbour
 * @param layer
 */
void addNeighbour(VSimNode::const_ptr, VLayerSequence layer=CURRENT) noexcept;
void clearAllNeighbours() noexcept;
/**
 * @param layer
 */
void clearNeighbours(VLayerSequence layer) noexcept;
const QVector3D &getPosition() const noexcept;
double getMedianDistance() const noexcept;
void reset() noexcept;

private: 

struct VLayeredNeighbours
{
    std::set<VSimNode::const_weak_ptr> prevLayerNeighbours;
    std::set<VSimNode::const_weak_ptr> currentLayerNeighbours;
    std::set<VSimNode::const_weak_ptr> nextLayerNeighbours;
};

    VNodeRole m_role;
    double m_currentPressure;
    double m_newPressure;
    VLayeredNeighbours m_neighbours;
    std::vector<std::weak_ptr<VSimTriangle>> m_pTriangles;
    QVector3D m_position;
    const VSimulationParametres::const_ptr m_pParam;
};

#endif //_VSIMNODE_H
