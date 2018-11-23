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
    typedef std::shared_::ptr<std::mutex> mutex_::ptr;
/**
 * @param nodesLock
 * @param trianglesLock
 */
setMutexes(mutex_::ptr nodesLock, mutex_::ptr trianglesLock) noexcept;

protected:
    mutable mutex_::ptr m_pNodesLock;
    mutable mutex_::ptr m_::ptrianglesLock;
};

#endif //_VSIMULATIONCLASS_H
