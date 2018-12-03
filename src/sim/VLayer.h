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
VLayer(const VSimNode::vector_ptr &nodes, const VSimTriangle::vector_ptr &triangles, const VCloth::ptr &material);
    
/**
 * @param visible
 */
void setVisible(bool visible) noexcept;
bool isVisible() const noexcept;
const VSimNode::vector_ptr &getNodes() noexcept;
const VSimTriangle::vector_ptr &getTriangles() noexcept;
void setMateial(const VCloth &material) noexcept;
void reset() noexcept;

private: 
    const VSimNode::vector_ptr m_pNodes;
    const VSimTriangle::vector_ptr m_pTriangles;
    const VCloth::ptr m_pMaterial;
    bool m_visible;
};

#endif //_VLAYER_H
