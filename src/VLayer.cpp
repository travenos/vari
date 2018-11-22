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
void VLayer::VLayer(vector<VNodePtr> nodes, vector<VTrianglePtr> triangles, VMaterial& material) {

}

/**
 * @param visible
 */
void VLayer::setVisible(bool visible) {

}

/**
 * @return bool
 */
bool VLayer::isVisible() {
    return false;
}

/**
 * @return double
 */
double VLayer::getMedianDistance() {
    return 0.0;
}

/**
 * @return vector<&VNodePtr>
 */
vector<&VNodePtr> VLayer::getNodes() {
    return null;
}

/**
 * @return vector<&VTrianglePtr>
 */
vector<&VTrianglePtr> VLayer::getTriangles() {
    return null;
}

void VLayer::reset() {

}