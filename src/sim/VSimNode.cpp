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
VSimNode::VSimNode(const QVector3D& pos,
                   const VCloth::const_ptr &p_material,
                   const VSimulationParametres::const_ptr &p_param):
    VSimElement(p_material, p_param),
    m_position(pos),
    m_neighboursNumber(0)
{

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
VSimNode::VNodeRole VSimNode::getRole() const  {
    return m_role;
}

void VSimNode::calculate()  {
    if(m_role == NORMAL) //we don't need to calculate a new pressure if we have constant pressure cause it's an injection point
    {
        double _K = m_pParam->getAveragePermeability();
        double K = m_pMaterial->permeability;
        double m = m_neighboursNumber;
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
        for(auto &it: m_neighbours.currentLayerNeighbours)
            calcNeighbour(it);
        for(auto &it: m_neighbours.previousLayerNeighbours)
            calcNeighbour(it);
        for(auto &it: m_neighbours.nextLayerNeighbours)
            calcNeighbour(it);

        m_newPressure = p_t+(brace0/m)*sum;
        if(m_newPressure < p_t)
            m_newPressure = p_t;
        if(m_newPressure >= m_pParam->getInjectionPressure())
            m_newPressure = m_pParam->getInjectionPressure();

        if(m_newPressure > highestNeighborPressure)
        {
            m_newPressure = highestNeighborPressure;
        }
    }
}

bool VSimNode::commit()  {
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
double VSimNode::getPressure() const  {
    return m_currentPressure;
}

/**
 * @param node
 * @param layer
 */
void VSimNode::addNeighbour(const VSimNode* node, VLayerSequence layer) 
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
    m_neighbours.currentLayerNeighbours.clear();
    m_neighbours.previousLayerNeighbours.clear();
    m_neighbours.nextLayerNeighbours.clear();
    m_neighboursNumber = 0;
}

/**
 * @param layer
 */
void VSimNode::clearNeighbours(VLayerSequence layer) 
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

double VSimNode::getDistance(const VSimNode * node) const 
{
    return m_position.distanceToPoint(node->getPosition());
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
    m_currentPressure = m_pParam->getVacuumPressure();
    m_newPressure = m_pParam->getVacuumPressure();
}

void VSimNode::getNeighbours(std::vector<const VSimNode*> &neighbours) const 
{
    neighbours.clear();
    neighbours.reserve(getNeighboursNumber());
    for (auto &neighbour : m_neighbours.currentLayerNeighbours)
        neighbours.push_back(neighbour.second);
    for (auto &neighbour : m_neighbours.previousLayerNeighbours)
        neighbours.push_back(neighbour.second);
    for (auto &neighbour : m_neighbours.nextLayerNeighbours)
        neighbours.push_back(neighbour.second);
}

size_t VSimNode::getNeighboursNumber() const
{
    return m_neighboursNumber;
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
