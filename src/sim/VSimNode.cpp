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
VSimNode::VSimNode(QVector3D& pos, VMaterial::const_ptr p_material, VSimulationParametres::const_ptr p_param) {

}

/**
 * @param role
 */
void VSimNode::setRole(VNodeRole role) {

}

/**
 * @return VNodeRole
 */
VSimNode::VNodeRole VSimNode::getRole() const {
    return m_role;
}

void VSimNode::calculate() {

}

void VSimNode::commit() {

}

/**
 * @return double
 */
double VSimNode::getPressure() const {
    return m_currentPressure;
}

/**
 * @param neighbour
 * @param layer
 */
void VSimNode::addNeighbour(const_ptr, VLayerSequence layer) {

}

void VSimNode::clearAllNeighbours() {

}

/**
 * @param layer
 */
void VSimNode::clearNeighbours(VLayerSequence layer) {

}

/**
 * @return const QVector3D&
 */
const QVector3D& VSimNode::getPosition() const {
    return m_position;
}

/**
 * @return double
 */
double VSimNode::getMedianDistance() const{
    return 0.0;
}

void VSimNode::reset() {

}
