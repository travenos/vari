/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSVIEWER_H
#define _VGRAPHICSVIEWER_H

#include <vector>
#include <memory>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include "VSimulationClass.h"
#include "VGraphicsNode.h"
#include "VGraphicsTriangle.h"

class VGraphicsViewer: public SoQtExaminerViewer, protected VSimulationClass {
public: 
    typedef std::shared_::ptr<VGraphicsViewer> ::ptr;
    typedef std::shared_::ptr<const VGraphicsViewer> const_::ptr;
    /**
     * @param simulator
     */
    VGraphicsViewer(VSimulator::ptr simulator);
    void updateConfiguration() noexcept;
    void updateTriangleColors() noexcept;
    void clearNodes() noexcept;
    void clearTriangles() noexcept;
    void clearAll() noexcept;
private: 
    std::vector<VGraphicsNode*> m_graphicsNodes;
    std::vector<VGraphicsTriangle*> m_graphicsTriangles;
    VSimulator::ptr m_pSimulator;
};

#endif //_VGRAPHICSVIEWER_H
