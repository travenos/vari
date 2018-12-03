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
        double time = 0;
        double filledPercent = 0;
        double averagePressure = 0;
        int iteration = 0;
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
    void start() noexcept;
    /**
 * Finish the simulation using a flag m_stopFlag
 */
    void stop() noexcept;
    /**
 * Check if simulation thread is currently active
 */
    bool isSimulating() const noexcept;
    void reset() noexcept;
    void clear() noexcept;
    void resetInfo() noexcept;
    /**
 * Update an information about active nodes and triangles (m_activeNodes, m_triangles)
 * @param layers
 */
    void setActiveElements(const VSimNode::const_vector_ptr &nodes,
                        const VSimTriangle::const_vector_ptr &triangles) noexcept(false);
    /**
 * Get the information about the current state of the simulation
 * @param info: output information about the current state of the simulation
 */
    VSimulationInfo getSimulationInfo() const noexcept;
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

    VSimulationParametres::const_ptr getSimulationParametres() const noexcept;

    void setInjectionDiameter(double diameter) noexcept;
    void setVacuumDiameter(double diameter) noexcept;
    void setViscosity(double viscosity) noexcept;
    void setVacuumPressure(double pressure) noexcept;
    void setInjectionPressure(double pressure) noexcept;
    void setQ(double q) noexcept;
    void setR(double r) noexcept;
    void setS(double s) noexcept;

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
    VSimNode::const_vector_ptr m_pActiveNodes;
     /**
     * Vector of all triangles which can be processed
     */
    VSimTriangle::const_vector_ptr m_pTriangles;
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
    //std::atomic<int> m_iteration; //TODO remove
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
     * A struct, containing all parametres needed for simulation
     */
    VSimulationParametres::ptr m_pParam;

    /**
     * A struct, containing all parametres, which describe current simulation status
     */
    VSimulationInfo m_info;

    /**
     * Lock, used to get access to structure with simulation info
     */
    mutable std::mutex m_infoLock;

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

    inline double timeDelta() const noexcept;
    inline double calcAveragePermeability() const noexcept;
    inline double calcAverageCellDistance() const noexcept;
};

#endif //_VSIMULATOR_H
