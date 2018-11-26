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
VLayer::VLayer(std::vector<VSimNode::ptr> &nodes, std::vector<VSimTriangle::ptr> &triangles, const VCloth& material) {

}

/**
 * @param visible
 */
void VLayer::setVisible(bool visible) noexcept {

}

/**
 * @return bool
 */
bool VLayer::isVisible() const noexcept {
    return false;
}

/**
 * @return double
 */
double VLayer::getMedianDistance() const  noexcept{
    return 0.0;
}

/**
 * @return vector<&VNode::ptr>
 */
std::vector<VSimNode::ptr> &VLayer::getNodes() noexcept {
    //return NULL;
}

/**
 * @return vector<&VTriangle::ptr>
 */
std::vector<VSimTriangle::ptr> &VLayer::getTriangles() noexcept {
    //return NULL;
}

void VLayer::setMateial(const VCloth &material) noexcept {

}

void VLayer::reset() noexcept {

}
