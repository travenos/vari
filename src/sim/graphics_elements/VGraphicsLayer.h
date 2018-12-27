/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSLAYER_H
#define _VGRAPHICSLAYER_H

#include <Inventor/nodes/SoSeparator.h>

#include "../VLayer.h"

class VGraphicsNode;
class VGraphicsTriangle;

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
    void reserveNodes(size_t n);
    void reserveTriangles(size_t n);
    void showInjectionPoint();
    void showVacuumPoint();
    void updateNodeColors();
    void updateTriangleColors();
    void updateVisibility();
    uint getNumber() const;
private:
    uint m_number;
    std::vector<VGraphicsNode*> m_graphicsNodes;
    std::vector<VGraphicsTriangle*> m_graphicsTriangles;
};

#endif //_VGRAPHICSLAYER_H
