/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VSimulationClass.h"

/**
 * VSimulationClass implementation
 */

VSimulationClass::VSimulationClass():
    m_pNodesLock(new std::mutex),
    m_pTrianglesLock(new std::mutex)
{}

void VSimulationClass::setMutexes(mutex_ptr nodesLock, mutex_ptr trianglesLock)  {
    m_pNodesLock = nodesLock;
    m_pTrianglesLock = trianglesLock;
}
