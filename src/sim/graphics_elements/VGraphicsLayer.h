/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSLAYER_H
#define _VGRAPHICSLAYER_H

#include <Inventor/nodes/SoSeparator.h>

#include "../sim_elements/VLayer.h"

class VGraphicsNode;
class VGraphicsTriangle;
class SoTransform;
class SbViewportRegion;

class VGraphicsLayer : public SoSeparator
{
public:
    VGraphicsLayer(const VLayer::const_ptr &simLayer, uint number);
    virtual ~VGraphicsLayer();
    VGraphicsLayer(const VGraphicsLayer& ) = delete;
    VGraphicsLayer& operator= (const VGraphicsLayer& ) = delete;
    inline void addNode(const VSimNode::const_ptr &simNode);
    inline void addTriangle(const VSimTriangle::const_ptr &simTriangle);
    void clearNodes();
    void clearTriangles();
    inline void reserveNodes(size_t n);
    inline void reserveTriangles(size_t n);
    void showInjectionPoint();
    void showVacuumPoint();
    void updateNodeColors();
    void updateTriangleColors();
    void updateVisibility();
    uint getNumber() const;
    bool isVisible() const;
    const SoTransform * getTransform() const;
    int getTransformId() const;
    std::shared_ptr<const std::vector<std::pair<uint, QVector3D> > > getNodesCoords(const SbViewportRegion & viewPortRegion, const SoPath * path) const;
private:
    uint m_number;
    bool m_visible;
    std::vector<std::pair<int, VGraphicsNode*> > m_graphicsNodes;
    std::vector<std::pair<int, VGraphicsTriangle*> > m_graphicsTriangles;
    SoTransform*  m_pTransform;
    int m_transformId;
};

#endif //_VGRAPHICSLAYER_H
