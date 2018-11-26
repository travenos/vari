/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSVIEWER_H
#define _VGRAPHICSVIEWER_H

#include <vector>
#include <memory>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include "VGraphicsNode.h"
#include "VGraphicsTriangle.h"
#include "VSimulator.h"

class SoExtSelection;
class SoIndexedLineSet;
class SoPointSet;

class VGraphicsViewer: public SoQtExaminerViewer, public VSimulationClass {
public: 
    typedef std::shared_ptr<VGraphicsViewer> ptr;
    typedef std::shared_ptr<const VGraphicsViewer> const_ptr;
    /**
     * @param simulator
     */
    VGraphicsViewer(QWidget* parent, VSimulator::ptr simulator);
    void updateConfiguration() noexcept;
    void updateTriangleColors() noexcept;
    void clearNodes() noexcept;
    void clearTriangles() noexcept;
    void clearAll() noexcept;
private: 
    void initGraph() noexcept;

    std::vector<VGraphicsNode*> m_graphicsNodes;
    std::vector<VGraphicsTriangle*> m_graphicsTriangles;
    VSimulator::ptr m_pSimulator;

    SoSeparator*        m_root;
    //TODO Remove all this code
    /*
    void initSelection() noexcept;
    SoSeparator* make_line_sg() noexcept;

    SoSeparator*        m_root;
    SoSeparator*        m_absRoot;
    SoBaseColor*        m_baseColor;
    SoSeparator*        m_shapeRoot;
    SoDrawStyle*        m_shapeDrawStyle;
    SoMaterial*         m_shapeMaterial;
    SoMaterial*         m_nodeMaterial;
    bool                m_fViewerIsEmpty;
    SoExtSelection*     m_pSelection;
    SoCoordinate3*      m_coords;
    SoIndexedLineSet*   m_lineset;
    SoCoordinate3*      m_pointcoords;
    SoPointSet*         m_pointset;
    */
};

#endif //_VGRAPHICSVIEWER_H
