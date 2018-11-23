/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VLayer.h"

/**
 * VLayer implementation
 */


/**
 * @param nodes
 * @param triangles
 * @param material
 */
VLayer::VLayer(std::vector<VNode::ptr> &nodes, std::vector<VTriangle::ptr> &triangles, const VMaterial& material) {

}

/**
 * @param visible
 */
void VLayer::setVisible(bool visible) {

}

/**
 * @return bool
 */
bool VLayer::isVisible() const {
    return false;
}

/**
 * @return double
 */
double VLayer::getMedianDistance() const{
    return 0.0;
}

/**
 * @return vector<&VNode::ptr>
 */
std::vector<VNode::ptr> &VLayer::getNodes() {
    return null::ptr;
}

/**
 * @return vector<&VTriangle::ptr>
 */
std::vector<VTriangle::ptr> &VLayer::getTriangles() {
    return null::ptr;
}

void VLayer::setMateial(const VMaterial &material){

}

void VLayer::reset() {

}
