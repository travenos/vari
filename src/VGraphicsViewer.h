/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSVIEWER_H
#define _VGRAPHICSVIEWER_H

#include "VSimulationClass.h"
#include "VGraphicsNode.h"
#include "VGraphicsTriangle.h"


class VGraphicsViewer: public VSimulationClass {
public: 
    
/**
 * @param simulator
 */
void VGraphicsViewer(VSimulatorPtr simulator);
    
void updateConfiguration();
    
void updateTriangleColors();
    
void clearNodes();
    
void clearTriangles();
    
void clearAll();
private: 
    vector<VGraphicsNode*> m_graphicsNodes;
    VGraphicsTriangle*[*] m_graphicsTriangles;
    VSimulatorPtr m_pSimulator;
    vector<VGraphicsNode> m_graphicsNodes;
    vector<VGraphicsTriangle> m_graphicsTriangles;
};

#endif //_VGRAPHICSVIEWER_H