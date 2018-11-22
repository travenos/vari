/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMULATIONCLASS_H
#define _VSIMULATIONCLASS_H

class VSimulationClass {
public: 
    
/**
 * @param nodesLock
 * @param trianglesLock
 */
void setMutexes(VMutexPtr nodesLock, VMutexPtr trianglesLock);
protected: 
    VMutexPtr m_pNodesLock;
    VMutexPtr m_pTrianglesLock;
};

#endif //_VSIMULATIONCLASS_H