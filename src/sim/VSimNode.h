/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMNODE_H
#define _VSIMNODE_H

#include <QVector3D>
#include <vector>
#include "VSimElement.h"
#include "VSimulationParametres.h"
#include "VSimTriangle.h"

class VSimNode: protected VSimElement {
public: 
    typedef std::shared_ptr<VSimNode> ptr;
    typedef std::shared_ptr<const VSimNode> const_ptr;

    enum VLayerSequence { PREVIOUS, CURRENT, NEXT };
    enum VNodeRole { NORMAL, INJECTION, VACUUM };
/**
 * @param pos
 * @param p_material
 * @param p_param
 */
VSimNode(QVector3D& pos, VMaterial::const_ptr p_material, VSimulationParametres::const_ptr p_param);
    
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
    std::vector<VSimNode::const_ptr> prevLayerNeighbours;
    std::vector<VSimNode::const_ptr> currentLayerNeighbours;
    std::vector<VSimNode::const_ptr> nextLayerNeighbours;
};

    VNodeRole m_role;
    double m_currentPressure;
    double m_newPressure;
    VLayeredNeighbours m_neighbours;
    std::vector<VSimTriangle::ptr> m_pTriangles;
    QVector3D m_position;
    const VSimulationParametres::const_ptr m_pParam;
};

#endif //_VSIMNODE_H
