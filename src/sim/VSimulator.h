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

class VSimulator: protected VSimulationClass {
public: 
    typedef std::shared_::ptr<VSimulator> ::ptr;
    typedef std::shared_::ptr<const VSimulator> const_::ptr;

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
void setActiveNodes(std::vector<VLayer::::ptr>& layers) noexcept;
void getSimulationInfo(VSimulationInfo &info) const noexcept;
void createGraphicsNodes() noexcept;
int getIterationNumber() const noexcept;

private: 
    /**
 * Vector of pointers to all nodes which can be processed
 */
std::vector<VSimNode::::ptr> m_activeNodes;
    /**
 * Vector of all triangles which can be processed
 */
std::vector<VSimTriangle::::ptr> m_triangles;
    /**
 * A thread, where simulation cycle is being processed
 */
std::unique_::ptr<std::thread> m_pSimulationThread;
    /**
 * Flag, representing if the simulation process is currently active
 */
std::atomic<bool> m_simulatingFlag;
    /**
 * Flag, used to stop the simulation
 */
std::atomic<bool> m_stopFlag;
    std::atomic<int> m_iteration;
    std::vector<VNode::ptr> m_activeNodes;
    
/**
 * A function, which is being executed in the simulation thread
 */
void simulationCycle();
    
void calculateNodes();
    
void commitNodes();
    
void setTriangleColors();
};

typedef VSimulator::ptr std::shared_::ptr<VSimulator>;

#endif //_VSIMULATOR_H
