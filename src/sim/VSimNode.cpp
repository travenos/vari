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
    VSimElement(p_material)
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

void VSimNode::commit() noexcept {

}

/**
 * @return double
 */
double VSimNode::getPressure() const noexcept {
    return m_currentPressure;
}

/**
 * @param neighbour
 * @param layer
 */
void VSimNode::addNeighbour(const_ptr, VLayerSequence layer) noexcept {

}

void VSimNode::clearAllNeighbours() noexcept {

}

/**
 * @param layer
 */
void VSimNode::clearNeighbours(VLayerSequence layer) noexcept {

}

/**
 * @return const QVector3D&
 */
const QVector3D& VSimNode::getPosition() const noexcept {
    return m_position;
}

/**
 * @return double
 */
double VSimNode::getMedianDistance() const noexcept{
    return 0.0;
}

void VSimNode::reset() noexcept {

}
