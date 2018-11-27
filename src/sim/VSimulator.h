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
#include "VGraphicsNode.h"
#include "VGraphicsTriangle.h"
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
 * Destructor
 */
    virtual ~VSimulator();
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
    /**
 * Get the information about the current state of the simulation
 * @param info: output information about the current state of the simulation
 */
    void getSimulationInfo(VSimulationInfo &info) const noexcept;
    /**
 * Create Graphic elements for VGraphicsView: nodes and triangles, connected to simulation elements
 * @param nodes_p
 * @param triangles_p
 */
    void createGraphicsElements(std::vector<VGraphicsNode *>& nodes_p,
                                std::vector<VGraphicsTriangle *>& triangles_p) const noexcept (false);
    /**
 * Get number of current iteration
 * @return int
 */
    int getIterationNumber() const noexcept;
    /**
 * Wait until some nodes state is changed
 */
    void waitForNewData() const noexcept;
    /**
 * Make waiting thread stop waiting and make it think that the simulation state has changed
 */
    void cancelWaitingForNewData() const noexcept;

private:
    typedef void(*nodeFunc)(VSimNode::ptr& node);
    typedef void(*triangleFunc)(VSimTriangle::ptr& triangle);

    /**
     * Number of threads for calculations during the simulation
     */
    static const unsigned int N_THREADS;

    /**
     * Number of nodes, which are calculated in the same thread
     */
    unsigned int m_nodesThreadPart;

    /**
     * Number of triangles, which are calculated in the same thread
     */
    unsigned int m_trianglesThreadPart;
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
    /**
     * Number of the current iteration
     */
    std::atomic<int> m_iteration;
    /**
    * This mutex is unlocked when the data is changed. It is needed for synchronization
    * with VGraphicsViewer. The methods waitForNewData() and cancelWaitingForNewData() are used.
    */
    mutable std::mutex m_newDataLock;
    /**
     * Vector used for storage of calculation threads
     */
    std::vector<std::thread> m_calculationThreads;

    /**
     * A function, which is being executed in the simulation thread
     */
    void simulationCycle()  noexcept;
    /**
     * Perform an action over all nodes
     * @param func Describes an action for node
     */
    template<typename Callable>
    inline void nodesAction(Callable &&func) noexcept;
    /**
     * Perform an action over all triangles
     * @param func Describes an action for triangle
     */
    template<typename Callable>
    inline void trianglesAction(Callable &&func) noexcept;

    template<typename T1, typename T2>
    inline void createGraphicsElementsCore(std::vector<T1 *> &gaphics,
                                           const std::vector< std::shared_ptr<T2> > &sim) const noexcept;
};

#endif //_VSIMULATOR_H
