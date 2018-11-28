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
VSimNode::VSimNode(QVector3D& pos, VCloth::const_ptr p_material, VSimulationParametres::const_ptr p_param):
    VSimElement(p_material, p_param),
    m_neighboursNumber(0)
{

}

/**
 * @param role
 */
void VSimNode::setRole(VNodeRole role) noexcept
{
    m_role = role;
}

/**
 * @return VNodeRole
 */
VSimNode::VNodeRole VSimNode::getRole() const noexcept {
    return m_role;
}

void VSimNode::calculate() noexcept {
    if(m_role == NORMAL) //we don't need to calculate a new pressure if we have constant pressure cause it's an injection point
    {
        double _K = m_pParam->averagePermeability;
        double K = m_pMaterial->permeability;
        double m = m_neighboursNumber;
        double phi = m_pMaterial->porosity;
        double d = m_pMaterial->cavityHeight;
        double _l = m_pParam->averageCellDistance;
        double q = m_pParam->q;
        double r = m_pParam->r;
        double s = m_pParam->s;
        double p_t = m_currentPressure;

        double brace0 = pow(K/_K,q);

        double sum=0;
        double highestNeighborPressure = 0;
        auto calcNeighbour = [d, phi, r, _l, s, p_t, &sum, &highestNeighborPressure]
                (const std::pair<double, const_weak_ptr> &it)
        {
            const_ptr neighbor = it.second.lock();
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
        for(auto &it: m_neighbours.currentLayerNeighbours)
            calcNeighbour(it);
        for(auto &it: m_neighbours.previousLayerNeighbours)
            calcNeighbour(it);
        for(auto &it: m_neighbours.nextLayerNeighbours)
            calcNeighbour(it);

        m_newPressure = p_t+(brace0/m)*sum;
        if(m_newPressure < p_t)
            m_newPressure = p_t;
        if(m_newPressure >= m_pParam->injectionPressure)
            m_newPressure = m_pParam->injectionPressure;

        if(m_newPressure > highestNeighborPressure)
        {
            m_newPressure = highestNeighborPressure;
        }
    }
}

bool VSimNode::commit() noexcept {
    if (m_newPressure != m_currentPressure)
    {
        m_currentPressure = m_newPressure;
        return true;
    }
    else
        return false;
    //TODO make like in original program
    /*
    if(m_pressure != m_memorizedPressure)
    {
        m_pressure = m_memorizedPressure;
        if(m_pressure > s_halfLimit)
        {
            if(m_pressure >= m_pSimulator->vacuumPressure())
            {
                if(getState() == SimNode::FULL)
                {
                    return FALSE;
                }
                else
                {
                    setState(FULL);
                }
            }
            else
            {
                setState(HALF);
            }
        }
        else
        {
            setState(EMPTY);
        }
        return TRUE;
    }
    else
    {
        return FALSE;
    }
    */
}

/**
 * @return double
 */
double VSimNode::getPressure() const noexcept {
    return m_currentPressure;
}

/**
 * @param node
 * @param layer
 */
void VSimNode::addNeighbour(const_ptr node, VLayerSequence layer) noexcept
{
    double dist = getDistance(node);
    switch (layer)
    {
    case CURRENT:
        m_neighbours.currentLayerNeighbours.insert(std::make_pair(dist, node));
        break;
    case PREVIOUS:
        m_neighbours.previousLayerNeighbours.insert(std::make_pair(dist, node));
        break;
    default:
        m_neighbours.nextLayerNeighbours.insert(std::make_pair(dist, node));
        break;
    }
    ++m_neighboursNumber;
}

void VSimNode::addNeighbourMutually(ptr node, VLayerSequence layer) noexcept
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
    node->addNeighbour(const_ptr(this), otherLayer);
}

void VSimNode::clearAllNeighbours() noexcept
{
    m_neighbours.currentLayerNeighbours.clear();
    m_neighbours.previousLayerNeighbours.clear();
    m_neighbours.nextLayerNeighbours.clear();
    m_neighboursNumber = 0;
}

/**
 * @param layer
 */
void VSimNode::clearNeighbours(VLayerSequence layer) noexcept
{
    switch (layer)
    {
    case CURRENT:
        m_neighbours.currentLayerNeighbours.clear();
        break;
    case PREVIOUS:
        m_neighbours.previousLayerNeighbours.clear();
        break;
    default:
        m_neighbours.nextLayerNeighbours.clear();
        break;
    }
    m_neighboursNumber =
            m_neighbours.currentLayerNeighbours.size()
            + m_neighbours.previousLayerNeighbours.size()
            + m_neighbours.nextLayerNeighbours.size();
}

double VSimNode::getDistance(VSimNode::const_ptr node) const noexcept
{
    return m_position.distanceToPoint(node->getPosition());
}

/**
 * @return const QVector3D&
 */
const QVector3D& VSimNode::getPosition() const noexcept
{
    return m_position;
}

void VSimNode::reset() noexcept
{
    m_currentPressure = m_pParam->vacuumPressure;
    m_newPressure = m_pParam->vacuumPressure;
}

void VSimNode::getNeighbours(std::vector<const_ptr> &neighbours) const noexcept
{
    neighbours.clear();
    neighbours.reserve(getNeighboursNumber());
    for (auto &neighbour : m_neighbours.currentLayerNeighbours)
        neighbours.push_back(neighbour.second.lock());
    for (auto &neighbour : m_neighbours.previousLayerNeighbours)
        neighbours.push_back(neighbour.second.lock());
    for (auto &neighbour : m_neighbours.nextLayerNeighbours)
        neighbours.push_back(neighbour.second.lock());
}

int VSimNode::getNeighboursNumber() const noexcept
{
    return m_neighboursNumber;
}

double VSimNode::getCavityHeight() const noexcept
{
    return m_pMaterial->cavityHeight;
}

double VSimNode::getPorosity() const noexcept
{
    return m_pMaterial->porosity;
}

double VSimNode::getPermeability() const noexcept
{
    return m_pMaterial->permeability;
}
