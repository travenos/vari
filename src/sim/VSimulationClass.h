/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMULATIONCLASS_H
#define _VSIMULATIONCLASS_H

#include <memory>
#include <mutex>

class VSimulationClass {
public: 
    typedef std::shared_ptr<std::mutex> mutex_ptr;
/**
 * @param nodesLock
 * @param trianglesLock
 */
void setMutexes(mutex_ptr nodesLock, mutex_ptr trianglesLock) noexcept;

protected:
    mutable mutex_ptr m_pNodesLock;
    mutable mutex_ptr m_pTrianglesLock;
};

#endif //_VSIMULATIONCLASS_H
