/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMULATOR_H
#define _VSIMULATOR_H

#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <QObject>
#include "VSimulationClass.h"
#include "VSimNode.h"
#include "VSimTriangle.h"
#include "VGraphicsNode.h"
#include "VGraphicsTriangle.h"
#include "VLayer.h"

class VSimulator: public QObject, public VSimulationClass
{
    Q_OBJECT
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
    void start() ;
    /**
 * Finish the simulation
 */
    void stop() ;
    /**
 * Pause the simulation
 */
    void pause() ;
    /**
 * Check if simulation thread is currently active
 */
    bool isSimulating() const ;
    /**
 * Check if simulation is set on pause
 */
    bool isPaused() const ;
    /**
 * Stop the simulation and reset all nodes states
 */
    void reset() ;
    /**
     * Stop the simulation and remove all pointers to simulation elements
     */
    void clear() ;
    /**
     * Reset information about the current simulation status
     */
    void resetInfo() ;
    /**
 * Update an information about active nodes and triangles (m_activeNodes, m_triangles)
 * @param layers
 */
    void setActiveElements(const VSimNode::const_vector_ptr &nodes,
                        const VSimTriangle::const_vector_ptr &triangles) ;
    /**
 * Get the information about the current state of the simulation
 * @param info: output information about the current state of the simulation
 */
    VSimulationInfo getSimulationInfo() const ;
    /**

 * Get number of current iteration
 * @return int
 */
    int getIterationNumber() const ;
    /**
 * Wait until some nodes state is changed
 */
    void waitForNewData() const ;
    /**
 * Make waiting thread stop waiting and make it think that the simulation state has changed
 */
    void cancelWaitingForNewData() const ;

    VSimulationParametres::const_ptr getSimulationParametres() const ;

    void setInjectionDiameter(double diameter) ;
    void setVacuumDiameter(double diameter) ;
    void setDefaultViscosity(double defaultViscosity) ;
    void setTemperature(double temperature) ;
    void setTempcoef(double tempcoef) ;
    void setVacuumPressure(double pressure) ;
    void setInjectionPressure(double pressure) ;
    void setQ(double q) ;
    void setR(double r) ;
    void setS(double s) ;

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
    size_t m_nodesThreadPart;

    /**
     * Number of triangles, which are calculated in the same thread
     */
    size_t m_trianglesThreadPart;
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
    * Flag, used to indicate, that simulation was paused
    */
    std::atomic<bool> m_pauseFlag;
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
    void simulationCycle()  ;

    /**
     * Interrupt the simulation using a flag m_stopFlag
     */
    void interrupt() ;
    /**
     * Perform an action over all nodes
     * @param func Describes an action for node
     */
    template<typename Callable>
    inline void nodesAction(Callable &&func) ;
    /**
     * Perform an action over all triangles
     * @param func Describes an action for triangle
     */
    template<typename Callable>
    inline void trianglesAction(Callable &&func) ;

    inline double timeDelta() const ;
    inline double calcAveragePermeability() const ;
    inline double calcAverageCellDistance() const ;

signals:
    void simulationStarted();
    void simulationPaused();
    void simulationStopped();
};

#endif //_VSIMULATOR_H
