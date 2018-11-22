/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMULATOR_H
#define _VSIMULATOR_H

#include "VSimulationClass.h"
#include "VNodePtr.h"


class VSimulator: public VSimulationClass {
public: 
    
/**
 * Default constructor
 */
void VSimulator();
    
/**
 * Start the simulation thread
 */
void startSimulation();
    
/**
 * Finish the simulation using a flag m_stopFlag
 */
void stopSimulation();
    
/**
 * Check if simulation thread is currently active
 */
bool isSimulating();
    
void reset();
    
/**
 * Update an information about active nodes and triangles (m_activeNodes, m_triangles)
 * @param layers
 */
void setActiveNodes(std::vector<VLayer>& layers);
    
void getSimulationInfo();
    
void createGraphicsNodes();
    
int getIterationNumber();
private: 
    /**
 * Vector of pointers to all nodes which can be processed
 */
vector<VNodePtr> m_activeNodes;
    /**
 * Vector of all triangles which can be processed
 */
vector<VTrianglePtr> m_triangles;
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
    vector<VNodePtr> m_activeNodes;
    
/**
 * A function, which is being executed in the simulation thread
 */
void simulationCycle();
    
void calculateNodes();
    
void commitNodes();
    
void setTriangleColors();
};

#endif //_VSIMULATOR_H