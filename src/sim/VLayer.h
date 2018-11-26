/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYER_H
#define _VLAYER_H

#include <vector>
#include <memory>
#include <QColor>
#include "VSimNode.h"
#include "VSimTriangle.h"

class VLayer {
public: 
    typedef std::shared_ptr<VLayer> ptr;
    typedef std::shared_ptr<const VLayer> const_ptr;
/**
 * @param nodes
 * @param triangles
 * @param material
 */
VLayer(std::vector<VSimNode::ptr> &nodes, std::vector<VSimTriangle::ptr> &triangles, const VCloth &material);
    
/**
 * @param visible
 */
void setVisible(bool visible) noexcept;
bool isVisible() const noexcept;
double getMedianDistance() const noexcept;
std::vector<VSimNode::ptr> &getNodes() noexcept;
std::vector<VSimTriangle::ptr> &getTriangles() noexcept;
void setMateial(const VCloth &material) noexcept;
void reset() noexcept;

private: 
    std::vector<VSimNode::ptr> m_nodes;
    std::vector<VSimTriangle::ptr> m_triangles;
    VCloth::ptr m_pMaterial;
    bool m_visible;
};

#endif //_VLAYER_H
