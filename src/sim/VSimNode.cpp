/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif

#include "VSimNode.h"

/**
 * VSimNode implementation
 */


VSimNode::VSimNode(uint id, const QVector3D& pos,
                   const VCloth::const_ptr &p_material,
                   double injectionPressure,
                   double vacuumPressure,
                   double pressure,
                   VNodeRole role):
    VSimElement(id, p_material),
    m_position(pos),
    m_injectionPressure(injectionPressure),
    m_vacuumPressure(vacuumPressure),
    m_currentPressure(pressure),
    m_newPressure(0),
    m_role(role),
    m_neighboursNumber(0),
    m_removeMark(false)
{
}

VSimNode::~VSimNode()
{
    #ifdef DEBUG_MODE
        qInfo() << "VSimNode destroyed";
    #endif
}

/**
 * @param role
 */
void VSimNode::setRole(VNodeRole role) 
{
    m_role = role;
}

VSimNode::VNodeRole VSimNode::getRole() const
{
    return m_role;
}

void VSimNode::setNewPressure(double newPressure)
{
    m_newPressure = newPressure;
}

void VSimNode::commit(bool *madeChanges, bool *isFull)
{
    if (isFull != nullptr)
        (*isFull) = (m_newPressure >= m_vacuumPressure);
    if(m_newPressure != m_currentPressure)
    {
        if (madeChanges != nullptr)
            (*madeChanges) = (m_currentPressure < m_vacuumPressure);
        m_currentPressure = m_newPressure;
    }
    else
    {
        if (madeChanges != nullptr)
            (*madeChanges) = false;
    }
}

double VSimNode::getPressure() const
{
    return m_currentPressure;
}

double VSimNode::getFilledPart() const
{
    double nom = m_currentPressure;
    double den = m_vacuumPressure;
    return (nom >= den) ? 1 : (nom / den);
}

void VSimNode::addNeighbour(VSimNode* node, VLayerSequence layer)
{
    double dist = getDistance(node);    
    m_neighbours[layer].push_back(std::make_pair(dist, node));
    ++m_neighboursNumber;
}

void VSimNode::addNeighbourMutually(VSimNode* node, VLayerSequence layer) 
{
    addNeighbour(node, layer);
    VLayerSequence otherLayer;
    switch (layer)
    {
    case CURRENT:
        otherLayer = CURRENT;
        break;
    case PREVIOUS:
        otherLayer = NEXT;
        break;
    default:
        otherLayer = PREVIOUS;
        break;
    }
    node->addNeighbour(this, otherLayer);
}

void VSimNode::removeNeighbour(uint id)
{
    for (uint i = 0; i < LAYERS_NUMBER; ++i)
    {
        auto it = m_neighbours[i].begin();
        while (it != m_neighbours[i].end())
        {
            if (it->second->getId() == id)
            {
                m_neighbours[i].erase(it++);
            }
            else
                ++it;
        }
    }
}

void VSimNode::removeNeighbour(const VSimNode* node)
{
    for (uint i = 0; i < LAYERS_NUMBER; ++i)
    {
        auto it = m_neighbours[i].begin();
        while (it != m_neighbours[i].end())
        {
            if (it->second == node)
            {
                m_neighbours[i].erase(it++);
            }
            else
                ++it;
        }
    }
}

void VSimNode::isolateNode()
{
    for (uint i = 0; i < LAYERS_NUMBER; ++i)
    {
        for (auto &neighbour : m_neighbours[i])
        {
            neighbour.second->removeNeighbour(this);
        }
        m_neighbours[i].clear();
    }
    m_neighboursNumber = 0;
}

void VSimNode::isolateAndMarkForRemove()
{
    isolateNode();
    m_removeMark = true;
}

bool VSimNode::isMarkedForRemove() const
{
    return m_removeMark;
}

void VSimNode::clearAllNeighbours() 
{
    m_neighbours[PREVIOUS].clear();
    m_neighbours[CURRENT].clear();
    m_neighbours[NEXT].clear();
    m_neighboursNumber = 0;
}

void VSimNode::clearNeighbours(VLayerSequence layer) 
{
    m_neighbours[layer].clear();
    m_neighboursNumber =
            m_neighbours[PREVIOUS].size()
            + m_neighbours[CURRENT].size()
            + m_neighbours[NEXT].size();
}

double VSimNode::getDistance(const VSimNode * node) const 
{
    return m_position.distanceToPoint(node->getPosition());
}

double VSimNode::getDistance(const QVector3D& point) const
{
    return m_position.distanceToPoint(point);
}

const QVector3D& VSimNode::getPosition() const 
{
    return m_position;
}

void VSimNode::reset()
{
    double pressure;
    if (!isInjection())
        pressure = 0;
    else
        pressure = m_injectionPressure;
    m_currentPressure = pressure;
    m_newPressure = pressure;
}

void VSimNode::setBoundaryPressures(double injectionPressure, double vacuumPressure)
{
    m_injectionPressure = injectionPressure;
    m_vacuumPressure = vacuumPressure;
}

void VSimNode::getNeighbours(std::vector<const VSimNode*> &neighbours) const 
{
    neighbours.clear();
    neighbours.reserve(getNeighboursNumber());
    for (auto &neighbour : m_neighbours[CURRENT])
        neighbours.push_back(neighbour.second);
    for (auto &neighbour : m_neighbours[PREVIOUS])
        neighbours.push_back(neighbour.second);
    for (auto &neighbour : m_neighbours[NEXT])
        neighbours.push_back(neighbour.second);
}

void VSimNode::getNeighbours(neighbours_list_t &neighbours) const
{
    neighbours.clear();
    for (uint layer = 0; layer < LAYERS_NUMBER; ++layer)
        std::copy(m_neighbours[layer].begin(), m_neighbours[layer].end(),
                  std::back_inserter(neighbours));
}

void VSimNode::getNeighbours(std::vector<const VSimNode*> &neighbours, VLayerSequence layer) const
{
    neighbours.clear();
    neighbours.reserve(getNeighboursNumber(layer));
    for (auto &neighbour : m_neighbours[layer])
        neighbours.push_back(neighbour.second);
}

void VSimNode::getNeighbours(neighbours_list_t &neighbours,
                             VLayerSequence layer) const
{
    neighbours.clear();
    std::copy(m_neighbours[layer].begin(), m_neighbours[layer].end(),
              std::back_inserter(neighbours));
}

const VSimNode::layered_neighbours_t &VSimNode::getNeighbours() const
{
    return m_neighbours;
}

const VSimNode::neighbours_list_t &VSimNode::getNeighbours(VLayerSequence layer) const
{
    return m_neighbours[layer];
}

void VSimNode::getNeighboursId(std::vector<uint> &neighbourId) const
{
    neighbourId.clear();
    neighbourId.reserve(getNeighboursNumber());
    for (auto &neighbour : m_neighbours[CURRENT])
        neighbourId.push_back(neighbour.second->getId());
    for (auto &neighbour : m_neighbours[PREVIOUS])
        neighbourId.push_back(neighbour.second->getId());
    for (auto &neighbour : m_neighbours[NEXT])
        neighbourId.push_back(neighbour.second->getId());
}

void VSimNode::getNeighboursId(std::vector<uint> &neighbourId, VLayerSequence layer) const
{
    neighbourId.clear();
    neighbourId.reserve(getNeighboursNumber());
    for (auto &neighbour : m_neighbours[layer])
        neighbourId.push_back(neighbour.second->getId());
}

size_t VSimNode::getNeighboursNumber() const
{
    return m_neighboursNumber;
}

size_t VSimNode::getNeighboursNumber(VLayerSequence layer) const
{
    return m_neighbours[layer].size();
}

double VSimNode::getCavityHeight() const 
{
    return m_pMaterial->cavityHeight;
}

double VSimNode::getPorosity() const 
{
    return m_pMaterial->porosity;
}

double VSimNode::getPermeability() const 
{
    return m_pMaterial->permeability;
}

bool VSimNode::isInjection() const
{
    return (m_role == INJECTION);
}

bool VSimNode::isVacuum() const
{
    return (m_role == VACUUM);
}

bool VSimNode::isNormal() const
{
    return (m_role == NORMAL);
}
