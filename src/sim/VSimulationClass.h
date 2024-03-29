/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMULATIONCLASS_H
#define _VSIMULATIONCLASS_H

#include <memory>
#include <mutex>
#include <QObject>

class VSimulationClass : public QObject
{
public: 
    typedef std::shared_ptr<std::mutex> mutex_ptr;
    VSimulationClass();
    virtual ~VSimulationClass() = default;
    VSimulationClass(const VSimulationClass& ) = delete;
    VSimulationClass& operator= (const VSimulationClass& ) = delete;
    VSimulationClass(VSimulationClass&& temp) = delete;
    VSimulationClass& operator=(VSimulationClass&& temp) = delete;

    void setMutexes(mutex_ptr nodesLock, mutex_ptr trianglesLock) ;

protected:
    mutable mutex_ptr m_pNodesLock;
    mutable mutex_ptr m_pTrianglesLock;
};

#endif //_VSIMULATIONCLASS_H
