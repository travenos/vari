/**
 * Project VARI
 * @author Alexey Barashkov
 */
#ifdef DEBUG_MODE
#include <QDebug>
#endif
#include <functional>
#include <QTime>
#include "VSimulator.h"
#include "VExceptions.h"

/**
 * VSimulator implementation
 */

const unsigned int VSimulator::TIMER_PERIOD = 1000;
/**
 * Number of threads for calculations during the simulation
 */
const unsigned int VSimulator::N_THREADS =
        (std::thread::hardware_concurrency()>1)?std::thread::hardware_concurrency()-1:1;

/**
 * Default constructor
 */
VSimulator::VSimulator():
    m_nodesThreadPart(1),
    m_trianglesThreadPart(1),
    m_pActiveNodes(new std::vector<VSimNode::ptr>),
    m_pTriangles(new std::vector<VSimTriangle::ptr>),
    m_simulatingFlag(false),
    m_pauseFlag(false),
    m_pParam(new VSimulationParametres),
    m_st_timeBeforePause(0)
{
    m_calculationThreads.resize(N_THREADS);
    m_calculationData.resize(N_THREADS);
}
/**
 * Destructor
 */
VSimulator::~VSimulator()
{
    stop();
}

/**
 * Start the simulation thread
 */
void VSimulator::start() 
{
    if (!isSimulating())
    {
        if (m_pSimulationThread && m_pSimulationThread->joinable())
                m_pSimulationThread->join();
        m_stopFlag.store(false);
        m_simulatingFlag.store(true);
        m_pSimulationThread.reset(new std::thread(std::bind(&VSimulator::simulationCycle, this)));
    }
}

/**
 * Finish the simulation using a flag m_stopFlag
 */
void VSimulator::stop() 
{
    bool wasOnPause = m_pauseFlag.load();
    m_pauseFlag.store(false);
    interrupt();
    if (wasOnPause)
        emit simulationStopped();
}

void VSimulator::pause()
{
    m_pauseFlag.store(true);
    interrupt();
}

void VSimulator::interrupt()
{
    m_stopFlag.store(true);
    if (m_pSimulationThread)
    {
        if (m_pSimulationThread->joinable())
            m_pSimulationThread->join();
        m_pSimulationThread.reset();
    }
}

/**
 * Check if simulation thread is currently active
 * @return bool
 */
bool VSimulator::isSimulating() const 
{
    return m_simulatingFlag.load();
}

bool VSimulator::isPaused() const
{
    return m_pauseFlag.load();
}

/**
 * Stop the simulation if it is active and set the default state for all nodes
 */
void VSimulator::reset() 
{
    stop();
    nodesAction([](const VSimNode::ptr& node){node->reset();});
    trianglesAction([](const VSimTriangle::ptr& triangle){triangle->reset();});
    resetInfo();
    m_newDataNotifier.notifyOne();
}

void VSimulator::clear() 
{
    stop();
    resetInfo();
    m_pActiveNodes.reset(new std::vector<VSimNode::ptr>);
    m_pTriangles.reset(new std::vector<VSimTriangle::ptr>);
}

VSimulationParametres::const_ptr VSimulator::getSimulationParametres() const 
{
    return m_pParam;
}

/**
 * Update an information about active nodes and triangles (m_activeNodes, m_triangles)
 * @param layers
 */
void VSimulator::setActiveElements(const VSimNode::const_vector_ptr &nodes,
                                const VSimTriangle::const_vector_ptr &triangles) 
{
    if (!isSimulating())
    {
        m_pActiveNodes = nodes;
        m_pTriangles = triangles;
        m_nodesThreadPart = m_pActiveNodes->size() / N_THREADS + 1;
        m_trianglesThreadPart = m_pTriangles->size() / N_THREADS + 1;
        m_pParam->setAverageCellDistance(getAverageCellDistance());
    }
    else
        throw VSimulatorException();
}

/**
 * Get the information about the current state of the simulation
 * @param info: output information about the current state of the simulation
 */
VSimulationInfo VSimulator::getSimulationInfo() const
{
    std::lock_guard<std::mutex> lock(m_infoLock);
    return m_info;
}

/**
 * Get number of current iteration
 * @return int
 */
int VSimulator::getIterationNumber() const  
{
    std::lock_guard<std::mutex> lock(m_infoLock);
    return m_info.iteration;
}
/**
 * Wait until some nodes state is changed
 */
void VSimulator::waitForNewData() const 
{
    m_newDataNotifier.wait();
}
/**
 * Make waiting thread stop waiting and make it think that the simulation state has changed
 */
void VSimulator::cancelWaitingForNewData() const 
{
    m_newDataNotifier.notifyOne();
}

/**
 * A function, which is being executed in the simulation thread
 */
void VSimulator::simulationCycle() 
{
    emit simulationStarted();
    QTime timeMeasurer;
    timeMeasurer.start();
    if (!(m_pauseFlag.load()))
    {
        m_pauseFlag.store(false);
        resetInfo();
        nodesAction([](const VSimNode::ptr& node){node->reset();});
        trianglesAction([](const VSimTriangle::ptr& triangle){triangle->reset();});
        m_pParam->setAveragePermeability(getAveragePermeability());
        m_pParam->setNumberOfFullNodes(0);
    }
    bool madeChangesInCycle = false;
    while(!(m_stopFlag.load()))
    {
        double filledPercent = getFilledPercent();
        double averagePressure = getAveragePressure();
        {
            std::lock_guard<std::mutex> locker(m_infoLock);
            double simTimeDelta = getTimeDelta();
            m_info.simTime += simTimeDelta;
            double oldRealTime = m_info.realTime;
            m_info.realTime = (m_st_timeBeforePause + timeMeasurer.elapsed()) / 1000.0;
            double realTimeDelta = m_info.realTime - oldRealTime;
            m_info.realtimeFactor = simTimeDelta / realTimeDelta;
            m_info.filledPercent = filledPercent;
            m_info.averagePressure = averagePressure;
            ++(m_info.iteration);
            #ifdef DEBUG_MODE
                qInfo() << "Time:" << m_info.simTime << "Iteration:" << m_info.iteration;
            #endif
        }
        calculatePressure();
        madeChangesInCycle = commitPressure();
        updateColors();
        m_newDataNotifier.notifyOne();
        if (!madeChangesInCycle)
            break;
    }
    m_simulatingFlag.store(false);
    if (!madeChangesInCycle)
        m_pauseFlag.store(false);
    if (!m_pauseFlag)
    {
        m_st_timeBeforePause = 0;
        emit simulationStopped();
    }
    else
    {
        m_st_timeBeforePause += timeMeasurer.elapsed();
        emit simulationPaused();
    }
    #ifdef DEBUG_MODE
        qInfo() << "The simulation was stopped";
    #endif
}

void VSimulator::resetInfo() 
{
    std::lock_guard<std::mutex> lock(m_infoLock);
    memset(&m_info, 0, sizeof(m_info));
}

template <typename Callable>
inline void VSimulator::nodesAction(Callable&& func) 
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    size_t batchStart = 0;
    size_t batchStop;
    for (size_t i = 0; i < N_THREADS; ++i)
    {
        batchStop = batchStart + m_nodesThreadPart;
        auto prc = [this, batchStart, batchStop, func]()
                {
                    for(size_t j = batchStart; j < batchStop && j < m_pActiveNodes->size(); ++j )
                        func((*m_pActiveNodes)[j]);
                };
        m_calculationThreads[i] = std::thread(prc);
        batchStart += m_nodesThreadPart;
    }
    for(auto &thread : m_calculationThreads)
        thread.join();
}

template <typename Callable>
inline double VSimulator::calcAverage(Callable&& func) const
{
    std::fill(m_calculationData.begin(), m_calculationData.end(), 0);
    size_t batchStart = 0;
    size_t batchStop;
    size_t nodesCount;
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        nodesCount = m_pActiveNodes->size();
        for (size_t i = 0; i < N_THREADS; ++i)
        {
            batchStop = batchStart + m_nodesThreadPart;
            auto prc = [this, batchStart, batchStop, nodesCount, i, func]()
                    {
                        for(size_t j = batchStart; j < batchStop && j < nodesCount; ++j )
                            m_calculationData[i] += func((*m_pActiveNodes)[j]);
                    };
            m_calculationThreads[i] = std::thread(prc);
            batchStart += m_nodesThreadPart;
        }
        for(auto &thread : m_calculationThreads)
            thread.join();
    }
    double sum = 0;
    for (auto& data : m_calculationData)
        sum += data;
    return (nodesCount > 0) ? sum / nodesCount : 0;
}

template <typename Callable>
inline void VSimulator::trianglesAction(Callable&& func)
{
    std::lock_guard<std::mutex> lock(*m_pTrianglesLock);
    size_t batchStart = 0;
    size_t batchStop;
    for (size_t i = 0; i < N_THREADS; ++i)
    {
        batchStop = batchStart + m_trianglesThreadPart;
        auto prc = [this, batchStart, batchStop, func]()
                {
                    for(size_t j = batchStart; j < batchStop && j < m_pTriangles->size(); ++j )
                        func((*m_pTriangles)[j]);
                };
        m_calculationThreads[i] = std::thread(prc);
        batchStart += m_trianglesThreadPart;
    }
    for(auto &thread : m_calculationThreads)
        thread.join();
}

inline void VSimulator::calculatePressure()
{
    auto calcFunc = [](const VSimNode::ptr& node)
    {node->calculate();};
    nodesAction(calcFunc);
}

inline bool VSimulator::commitPressure()
{
    std::atomic<bool> madeChangesInCycle(false);
    std::atomic<int> fullNodesCount(0);
    auto commitFunc = [&madeChangesInCycle, &fullNodesCount](const VSimNode::ptr& node)
    {
        bool madeChanges;
        bool isFull;
        node->commit(&madeChanges, &isFull);
        if(madeChanges && !madeChangesInCycle.load())
            madeChangesInCycle.store(true);
        if (isFull)
            ++fullNodesCount;
    };
    nodesAction(commitFunc);
    m_pParam->setNumberOfFullNodes(fullNodesCount);
    return madeChangesInCycle.load();
}

inline void VSimulator::updateColors()
{
    auto updateColorsFunc = [](const VSimTriangle::ptr& triangle){triangle->updateColor();};
    trianglesAction(updateColorsFunc);
}

inline double VSimulator::getFilledPercent() const
{
    auto filledPartFunc = [] (const VSimNode::ptr& node) -> double
    {
        return node->getFilledPart();
    };
    double filledPercent = calcAverage(filledPartFunc) * 100;
    return filledPercent;
}

inline double VSimulator::getAveragePressure() const
{
    auto pressureFunc = [] (const VSimNode::ptr& node) -> double
    {
        return node->getPressure();
    };
    return calcAverage(pressureFunc);
}

inline double VSimulator::getAveragePermeability() const
{
    auto permeabilityFunc = [] (const VSimNode::ptr& node) -> double
    {
        return node->getPermeability();
    };
    return calcAverage(permeabilityFunc);
}

inline double VSimulator::getAverageCellDistance() const
{
    double distance = 0;
    int counter = 0;
    std::vector<const VSimNode*> neighbours;
    for(auto &node : *m_pActiveNodes)
    {
        node->getNeighbours(neighbours);
        for(auto &neighbour : neighbours)
        {
            distance += node->getDistance(neighbour);
            ++counter;
        }
    }
    if (counter > 0)
        distance /= counter;
    return distance;
}

inline double VSimulator::getTimeDelta() const
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    if (m_pActiveNodes->size() > 0)
    {
        double n = m_pParam->getViscosity();                             //[N*s/m2]
        double _l = m_pParam->getAverageCellDistance();                   //[m]
        double l_typ = (sqrt((double)m_pParam->getNumberOfFullNodes())*_l);   //[m]
        double _K = m_pParam->getAveragePermeability();                   //[m^2]
        double p_inj = m_pParam->getInjectionPressure();          //[N/m2]
        double p_vac = m_pParam->getVacuumPressure();                    //[N/m2]

        double time = n*l_typ*_l/(_K*(p_inj-p_vac));
        return time;
    }
    else
        return 0;
}

void VSimulator::setResin(const VResin& resin)
{
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        m_pParam->setResin(resin);
    }
    emit resinChanged();
}

void VSimulator::setInjectionDiameter(double diameter) 
{
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        m_pParam->setInjectionDiameter(diameter);
    }
    emit injectionDiameterSet(diameter);
}

void VSimulator::setVacuumDiameter(double diameter) 
{
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        m_pParam->setVacuumDiameter(diameter);
    }
    emit vacuumDiameterSet(diameter);
}

void VSimulator::setTemperature(double temperature) 
{
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        m_pParam->setTemperature(temperature);
    }
    emit temperatureSet(temperature);
}

void VSimulator::setVacuumPressure(double pressure) 
{
    std::lock(*m_pNodesLock, *m_pTrianglesLock);
    m_pParam->setVacuumPressure(pressure);
    m_pNodesLock->unlock();
    m_pTrianglesLock->unlock();
    emit vacuumPressureSet(pressure);
}

void VSimulator::setInjectionPressure(double pressure) 
{
    std::lock(*m_pNodesLock, *m_pTrianglesLock);
    m_pParam->setInjectionPressure(pressure);
    m_pNodesLock->unlock();
    m_pTrianglesLock->unlock();
    emit injectionPressureSet(pressure);
}

void VSimulator::setQ(double q) 
{
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        m_pParam->setQ(q);
    }
    emit coefQSet(q);
}

void VSimulator::setR(double r) 
{
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        m_pParam->setR(r);
    }
    emit coefRSet(r);
}

void VSimulator::setS(double s) 
{
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        m_pParam->setS(s);
    }
    emit coefSSet(s);
}
