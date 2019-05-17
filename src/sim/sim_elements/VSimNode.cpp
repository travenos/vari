/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif

#include <QVector2D>

#include "VSimNode.h"

/**
 * VSimNode implementation
 */

const double VSimNode::CONSIDERED_FULL{0.98};

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

void VSimNode::commit(bool *p_madeChanges, bool *p_isFull)
{
    if (p_isFull != nullptr)
        (*p_isFull) = isFull();
    if(m_newPressure != m_currentPressure)
    {
        if (p_madeChanges != nullptr)
            (*p_madeChanges) = (m_currentPressure < m_vacuumPressure);
        m_currentPressure = m_newPressure;
    }
    else
    {
        if (p_madeChanges != nullptr)
            (*p_madeChanges) = false;
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

bool VSimNode::isFull() const
{
    return (getFilledPart() >= CONSIDERED_FULL);
}

void VSimNode::addNeighbour(VSimNode* node, VLayerSequence layer)
{
    double dist = getDistance(node);
    addNeighbour(node, layer, dist);
}

void VSimNode::addNeighbourMutually(VSimNode* node, VLayerSequence layer) 
{
    addNeighbour(node, layer);
    node->addNeighbour(this, layer);
}

void VSimNode::addNeighbour(const VSimNode::ptr &node, VLayerSequence layer)
{
    if (node)
        addNeighbour(node.get(), layer);
}

void VSimNode::addNeighbourMutually(const VSimNode::ptr &node, VLayerSequence layer)
{
    if (node)
        addNeighbourMutually(node.get(), layer);
}

void VSimNode::addNeighbour(VSimNode* node, VLayerSequence layer, float dist)
{
    if (node == this)
        return;
    for (auto &neighbour: m_neighbours[layer])
    {
        if (neighbour.second == node)
            return;
    }
    m_neighbours[layer].push_back(std::make_pair(dist, node));
    ++m_neighboursNumber;
}

void VSimNode::addNeighbourMutually(VSimNode* node, VLayerSequence layer, float dist)
{
    addNeighbour(node, layer, dist);
    node->addNeighbour(this, layer, dist);
}

void VSimNode::addNeighbour(const VSimNode::ptr &node, VLayerSequence layer, float dist)
{
    if (node)
        addNeighbour(node.get(), layer, dist);
}

void VSimNode::addNeighbourMutually(const VSimNode::ptr &node, VLayerSequence layer, float dist)
{
    if (node)
        addNeighbourMutually(node.get(), layer, dist);
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
                --m_neighboursNumber;
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
                --m_neighboursNumber;
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
    for (uint i = 0; i < LAYERS_NUMBER; ++i)
        m_neighbours[i].clear();
    m_neighboursNumber = 0;
}

void VSimNode::clearNeighbours(VLayerSequence layer) 
{
    m_neighbours[layer].clear();
    m_neighboursNumber = calcNeighboursNumber();
}

void VSimNode::clearAllNeighboursMutually()
{
    isolateNode();
}

void VSimNode::clearNeighboursMutually(VLayerSequence layer)
{
    for (auto &neighbour : m_neighbours[layer])
    {
        neighbour.second->removeNeighbour(this);
    }
    m_neighbours[layer].clear();
    m_neighboursNumber = calcNeighboursNumber();
}

float VSimNode::getDistance(const QVector2D &point) const
{
    QVector2D xyPosition = m_position.toVector2D();
    return xyPosition.distanceToPoint(point);
}

void VSimNode::setPosition(const QVector3D& pos)
{
    m_position = pos;
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
    for (uint i = 0; i < LAYERS_NUMBER; ++i)
    {
        for (auto &neighbour : m_neighbours[i])
            neighbours.push_back(neighbour.second);
    }
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
    for (uint i = 0; i < LAYERS_NUMBER; ++i)
    {
        for (auto &neighbour : m_neighbours[i])
            neighbourId.push_back(neighbour.second->getId());
    }
}

void VSimNode::getNeighboursId(std::vector<uint> &neighbourId, VLayerSequence layer) const
{
    neighbourId.clear();
    neighbourId.reserve(getNeighboursNumber());
    for (auto &neighbour : m_neighbours[layer])
        neighbourId.push_back(neighbour.second->getId());
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

bool VSimNode::isIsotropic() const
{
    return (m_pMaterial->getXPermeability() == m_pMaterial->getYPermeability());
}

inline size_t VSimNode::calcNeighboursNumber() const
{
    size_t size = 0;
    for (uint i = 0; i < LAYERS_NUMBER; ++i)
        size += m_neighbours[i].size();
    return size;
}
