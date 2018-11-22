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
 * @param material
 * @param param
 */
void VSimNode::VSimNode(QVector3D& pos, VMaterialPtr material, VSimParamPtr param) {

}

/**
 * @param role
 */
void VSimNode::setRole(VNodeRole role) {

}

/**
 * @return VNodeRole
 */
VNodeRole VSimNode::getRole() {
    return null;
}

void VSimNode::calculate() {

}

void VSimNode::commit() {

}

/**
 * @return double
 */
double VSimNode::getPressure() {
    return 0.0;
}

/**
 * @param neighbour
 * @param layer
 */
void VSimNode::addNeighbour(const VSimNode* neighbour, VLayerSequence layer) {

}

void VSimNode::clearAllNeighbours() {

}

/**
 * @param layer
 */
void VSimNode::clearNeighbours(VLayerSequence layer) {

}

/**
 * @return QVector3D&
 */
QVector3D& VSimNode::getPosition() {
    return null;
}

/**
 * @return double
 */
double VSimNode::getMedianDistance() {
    return 0.0;
}

void VSimNode::reset() {

}