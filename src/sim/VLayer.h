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

struct VMaterial
{
    typedef std::shared_ptr<VMaterial> ptr;
    typedef std::shared_ptr<const VMaterial> const_ptr;
    double cavityHeight;
    double permability;
    double porosity;
    QColor baseColor;
};

class VLayer {
public: 
    typedef std::shared_ptr<VLayer> ptr;
    typedef std::shared_ptr<const VLayer> const_ptr;
/**
 * @param nodes
 * @param triangles
 * @param material
 */
VLayer(std::vector<VSimNode::ptr> &nodes, std::vector<VSimTriangle::ptr> &triangles, const VMaterial &material);
    
/**
 * @param visible
 */
void setVisible(bool visible) noexcept;
bool isVisible() const noexcept;
double getMedianDistance() const noexcept;
std::vector<VSimNode::ptr> &getNodes() noexcept;
std::vector<VSimTriangle::ptr> &getTriangles() noexcept;
void setMateial(const VMaterial &material) noexcept;
void reset() noexcept;

private: 
    std::vector<VSimNode::ptr> m_nodes;
    std::vector<VSimTriangle::ptr> m_triangles;
    VMaterial::ptr m_pMaterial;
    bool m_visible;
};

#endif //_VLAYER_H
