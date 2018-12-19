/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include <cmath>
#include "VSimNode.h"

/**
 * VSimNode implementation
 */


/**
 * @param pos
 * @param p_material
 * @param p_param
 */
VSimNode::VSimNode(uint id, const QVector3D& pos,
                   const VCloth::const_ptr &p_material,
                   const VSimulationParametres::const_ptr &p_param):
    VSimElement(id, p_material, p_param),
    m_role(NORMAL),
    m_position(pos),
    m_neighboursNumber(0)
{
    reset();
}

/**
 * @param role
 */
void VSimNode::setRole(VNodeRole role) 
{
    m_role = role;
}

/**
 * @return VNodeRole
 */
VSimNode::VNodeRole VSimNode::getRole() const
{
    return m_role;
}

void VSimNode::calculate()
{
    double m = m_neighboursNumber;
    if(!isInjection() && m > 0) //we don't need to calculate a new pressure if we have constant pressure cause it's an injection point
    {
        double _K = m_pParam->getAveragePermeability();
        double K = m_pMaterial->permeability;
        double phi = m_pMaterial->porosity;
        double d = m_pMaterial->cavityHeight;
        double _l = m_pParam->getAverageCellDistance();
        double q = m_pParam->getQ();
        double r = m_pParam->getR();
        double s = m_pParam->getS();
        double p_t = m_currentPressure;

        double brace0 = pow(K/_K,q);

        double sum=0;
        double highestNeighborPressure = 0;
        auto calcNeighbour = [d, phi, r, _l, s, p_t, &sum, &highestNeighborPressure]
                (const std::pair<double, const VSimNode* > &it)
        {
            const VSimNode* neighbor = it.second;
            double distance = it.first;
            double d_i = neighbor->getCavityHeight();
            double phi_i = neighbor->getPorosity();
            double brace1 = pow(((d_i*phi_i)/(d*phi)),r);
            double brace2 = pow(_l/distance,s);
            double p_it = neighbor->getPressure();
            double brace3 = p_it-p_t;
            sum += (brace1*brace2*brace3);
            if(p_it > highestNeighborPressure)
            {
                highestNeighborPressure = p_it;
            }
        };
        for(auto &it: m_neighbours[CURRENT])
            calcNeighbour(it);
        for(auto &it: m_neighbours[PREVIOUS])
            calcNeighbour(it);
        for(auto &it: m_neighbours[NEXT])
            calcNeighbour(it);

        m_newPressure = p_t+(brace0/m)*sum;
        if(m_newPressure < p_t)
            m_newPressure = p_t;
        if(m_newPressure >= m_pParam->getInjectionPressure())
            m_newPressure = m_pParam->getInjectionPressure();

        if (!isVacuum())
        {
            if(m_newPressure > highestNeighborPressure)
            {
                m_newPressure = highestNeighborPressure;
            }
        }
        else
        {
            if(m_newPressure > m_pParam->getVacuumPressure())
            {
                m_newPressure = m_pParam->getVacuumPressure();
            }
        }
    }
}

void VSimNode::commit(bool *madeChanges, bool *isFull)
{
    if (isFull != nullptr)
        (*isFull) = (m_newPressure >= m_pParam->getVacuumPressure());
    if(m_newPressure != m_currentPressure)
    {
        if (madeChanges != nullptr)
            (*madeChanges) = (m_currentPressure < m_pParam->getVacuumPressure());
        m_currentPressure = m_newPressure.load();
    }
    else
    {
        if (madeChanges != nullptr)
            (*madeChanges) = false;
    }
}

/**
 * @return double
 */
double VSimNode::getPressure() const
{
    return m_currentPressure;
}

double VSimNode::getFilledPart() const
{
    double nom = m_currentPressure;
    double den = m_pParam->getVacuumPressure();
    return (nom >= den) ? 1 : (nom / den);
}

double VSimNode::getNewPressure() const
{
    return m_newPressure;
}

/**
 * @param node
 * @param layer
 */
void VSimNode::addNeighbour(const VSimNode* node, VLayerSequence layer) 
{
    double dist = getDistance(node);    
    m_neighbours[layer].insert(std::make_pair(dist, node));
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

void VSimNode::clearAllNeighbours() 
{
    m_neighbours[PREVIOUS].clear();
    m_neighbours[CURRENT].clear();
    m_neighbours[NEXT].clear();
    m_neighboursNumber = 0;
}

/**
 * @param layer
 */
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

/**
 * @return const QVector3D&
 */
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
        pressure = m_pParam->getInjectionPressure();
    m_currentPressure = pressure;
    m_newPressure = pressure;
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

void VSimNode::getNeighbours(std::vector<const VSimNode*> &neighbours, VLayerSequence layer) const
{
    neighbours.clear();
    neighbours.reserve(getNeighboursNumber(layer));
    for (auto &neighbour : m_neighbours[layer])
        neighbours.push_back(neighbour.second);
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
