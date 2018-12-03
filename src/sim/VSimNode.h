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
void setRole(VNodeRole role) noexcept;
VNodeRole getRole() const noexcept;
void calculate() noexcept;
bool commit() noexcept;
double getPressure() const noexcept;
/**
 * @param neighbour
 * @param layer
 */
void addNeighbourMutually(VSimNode *node, VLayerSequence layer=CURRENT) noexcept;
void addNeighbour(const VSimNode *node, VLayerSequence layer=CURRENT) noexcept;
void clearAllNeighbours() noexcept;
/**
 * @param layer
 */
void clearNeighbours(VLayerSequence layer) noexcept;

double getDistance(const VSimNode* node) const noexcept;
const QVector3D &getPosition() const noexcept;
void reset() noexcept override;
void getNeighbours(std::vector<const VSimNode *> &neighbours) const noexcept;
int getNeighboursNumber() const noexcept;
double getCavityHeight() const noexcept;
double getPorosity() const noexcept;
double getPermeability() const noexcept;

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
    int m_neighboursNumber;
};

#endif //_VSIMNODE_H
