/**
 * Project VARI
 * @author Alexey Barashkov
 */


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
    VSimElement(p_material),
    m_neighboursNumber(0)
{

}

/**
 * @param role
 */
void VSimNode::setRole(VNodeRole role) noexcept {

}

/**
 * @return VNodeRole
 */
VSimNode::VNodeRole VSimNode::getRole() const noexcept {
    return m_role;
}

void VSimNode::calculate() noexcept {

}

bool VSimNode::commit() noexcept {
    if (m_newPressure != m_currentPressure)
    {
        m_currentPressure = m_newPressure;
        return true;
    }
    else
        return false;
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
    m_currentPressure = 0;
    m_newPressure = 0;
    //TODO assign value of defaultPressure from VSimulationParametres
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
