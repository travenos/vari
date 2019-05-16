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

#include "VSimulationClass.h"
#include "sim_elements/VSimNode.h"
#include "sim_elements/VSimTriangle.h"
#include "structures/VNotify.h"
#include "structures/VSimulationInfo.h"

class VSimulator: public VSimulationClass
{
    Q_OBJECT
public: 
    typedef std::shared_ptr<VSimulator> ptr;
    typedef std::shared_ptr<const VSimulator> const_ptr;

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
    * Check if time timit mode is on
    */
    bool isTimeLimitModeOn() const ;

    bool isLifetimeConsidered() const;

    bool isVacuumFullLimited() const;

    /**
     * Stop the simulation and reset all nodes states
     */
    void reset() ;
    /**
     * Stop the simulation and remove all pointers to simulation elements
     */
    void clear() ;
    /**
     * Reset all nodes, triangles and information about the simulation status
     */
    void resetState();
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

    /**
     * Set default color for the triangles, not dependant on simulation state
     */
    void resetTriangleColors();

    VSimulationParameters getSimulationParameters() const ;

    /**
     * Set simulation state
     * @param param
     * @param info
     * @param isPaused
     */
    void setSimulationParameters(const VSimulationInfo &info,
                                 const VSimulationParameters &param,
                                 bool isPaused=false, bool isTimeLimited=false);

    void setResin(const VResin &resin) ;
    void setInjectionDiameter(float diameter) ;
    void setVacuumDiameter(float diameter) ;
    void setTemperature(double temperature) ;
    void setVacuumPressure(double pressure) ;
    void setInjectionPressure(double pressure) ;
    void setQ(double q) ;
    void setR(double r) ;
    void setS(double s) ;
    void setTimeLimit(double limit);
    void setTimeLimitMode(bool on);
    void considerLifetime(bool on);
    void stopOnVacuumFull(bool on);

private:
    typedef void(*nodeFunc)(VSimNode::ptr& node);
    typedef void(*triangleFunc)(VSimTriangle::ptr& triangle);

    /**
     * Number of threads for calculations during the simulation
     */
    static const uint N_THREADS;

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

    VSimNode::list_ptr m_pVacuumNodes;
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
    * Flags used for time limitation modes
    */
    std::atomic<bool> m_timeLimitFlag;

    std::atomic<bool> m_lifetimeLimitFlag;

    std::atomic<bool> m_vacuumFullLimitFlag;
    /**
     * Vector used for storage of calculation threads
     */
    mutable std::vector<std::thread> m_calculationThreads;
    /**
     * Vector used for storage of calculated data
     */
    mutable std::vector<double> m_calculationData;

    /**
     * A struct, containing all parameters needed for simulation.
     * Should be accessed only when m_pNodesLock is locked
     */
    VSimulationParameters m_param;

    /**
     * A struct, containing all parameters, which describe current simulation status
     */
    VSimulationInfo m_info;
    /**
     * Lock, used to get access to structure with simulation info
     */
    mutable std::mutex m_infoLock;

    /**
    * Object for notification when the data is changed.
    * It is needed for synchronization with VGraphicsViewer.
    * The methods waitForNewData() and cancelWaitingForNewData() are used.
    */
    mutable VNotify m_newDataNotifier;

    /**
     * Duration of the simulation process before pause. Should be accessed only from simulation thread.
     */
    int m_simT_timeBeforePause;

    std::atomic<bool> m_destroyed;

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

    /**
     * Calculate some average value for all nodes
     * @param func Describes an action for node
     */
    template <typename Callable>
    inline double calcAverage(Callable&& func) const;

    inline void calculatePressure();
    inline bool moveToNextStep();
    inline void updateColors();

    void calculateNewPressure(const VSimNode::ptr &node);
    inline double getRelativePermeability(const VSimNode* node, const VSimNode* neighbor) const;
    inline bool isInsideLongitudinal(const VSimNode* node, const VSimNode* neighbour) const;
    inline bool isInsideTraversal(const VSimNode* node, const VSimNode* neighbour) const;
    inline double mf_pow(double x, double y) const;

    inline double getTimeDelta() const ;
    inline double getAveragePermeability() const ;
    inline double getAverageCellDistance() const ;
    inline double getFilledPercent() const;
    inline double getAveragePressure() const;


signals:
    void simulationStarted();
    void simulationPaused();
    void simulationStopped();

    void resinChanged();
    void injectionDiameterSet(float diameter) ;
    void vacuumDiameterSet(float diameter) ;
    void temperatureSet(double temperature) ;
    void vacuumPressureSet(double pressure) ;
    void injectionPressureSet(double pressure) ;
    void coefQSet(double q) ;
    void coefRSet(double r) ;
    void coefSSet(double s) ;

    void timeLimitModeSwitched(bool);
    void lifetimeConsiderationSwitched(bool);
    void stopOnVacuumFullSwitched(bool);
    void timeLimitSet(double);
};

#endif //_VSIMULATOR_H
