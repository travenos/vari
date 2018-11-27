/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMULATOR_H
#define _VSIMULATOR_H

#include <vector>
#include <memory>
#include <thread>
#include "VSimulationClass.h"
#include "VSimNode.h"
#include "VSimTriangle.h"
#include "VLayer.h"

class VSimulator: public VSimulationClass {
public: 
    typedef std::shared_ptr<VSimulator> ptr;
    typedef std::shared_ptr<const VSimulator> const_ptr;

    struct VSimulationInfo
    {
        double time;
        double filledPercent;
        double medianPressure;
    };
/**
 * Default constructor
 */
VSimulator();
/**
 * Start the simulation thread
 */
void startSimulation() noexcept;
/**
 * Finish the simulation using a flag m_stopFlag
 */
void stopSimulation() noexcept;
/**
 * Check if simulation thread is currently active
 */
bool isSimulating() const noexcept;
void reset() noexcept;
/**
 * Update an information about active nodes and triangles (m_activeNodes, m_triangles)
 * @param layers
 */
void setActiveNodes(std::vector<VLayer::ptr>& layers) noexcept(false);
void getSimulationInfo(VSimulationInfo &info) const noexcept;
void createGraphicsNodes() noexcept (false);
int getIterationNumber() const noexcept;
void waitForNewData() const noexcept;
void cancelWaitingForNewData() const noexcept;

private: 
    /**
 * Vector of pointers to all nodes which can be processed
 */
std::vector<VSimNode::ptr> m_activeNodes;
    /**
 * Vector of all triangles which can be processed
 */
std::vector<VSimTriangle::ptr> m_triangles;
    /**
 * A thread, where simulation cycle is being processed
 */
std::unique_ptr<std::thread> m_pSimulationThread;
    /**
 * Flag, representing if the simulation process is currently active
 */
std::atomic<bool> m_simulatingFlag;
    /**
 * Flag, used to stop the simulation
 */
std::atomic<bool> m_stopFlag;
    std::atomic<int> m_iteration;
    
    mutable std::mutex m_newDataLock;
/**
 * A function, which is being executed in the simulation thread
 */
void simulationCycle()  noexcept;    
void calculateNodes() noexcept;    
void commitNodes() noexcept;    
void setTriangleColors() noexcept;
};

#endif //_VSIMULATOR_H
