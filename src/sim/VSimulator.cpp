/**
 * Project VARI
 * @author Alexey Barashkov
 */
#ifdef DEBUG_MODE
#include <QDebug>
#endif
#include <functional>
#include <cstring>
#include <cmath>
#include <QTime>

#include "VSimulator.h"
#include "structures/VExceptions.h"
#include "structures/VSimulationParameters.h"

/**
 * VSimulator implementation
 */

const uint VSimulator::N_THREADS =
        (std::thread::hardware_concurrency()>1)?std::thread::hardware_concurrency()-1:1;

VSimulator::VSimulator():
    m_nodesThreadPart(1),
    m_trianglesThreadPart(1),
    m_pActiveNodes(new VSimNode::vector_t),
    m_pVacuumNodes(new VSimNode::list_t),
    m_pTriangles(new VSimTriangle::vector_t),
    m_simulatingFlag(false),
    m_pauseFlag(false),
    m_timeLimitFlag(false),
    m_lifetimeLimitFlag(true),
    m_simT_timeBeforePause(0),
    m_destroyed(false)
{
    m_calculationThreads.resize(N_THREADS);
    m_calculationData.resize(N_THREADS);
}

VSimulator::~VSimulator()
{
    m_destroyed = true;
    stop();
}

void VSimulator::start() 
{
    if (!isSimulating())
    {
        if (m_pSimulationThread && m_pSimulationThread->joinable())
                m_pSimulationThread->join();
        m_stopFlag.store(false);
        m_simulatingFlag.store(true);
        m_pSimulationThread.reset(new std::thread(&VSimulator::simulationCycle, this));
    }
}

void VSimulator::stop() 
{
    bool wasOnPause = m_pauseFlag.load();
    m_pauseFlag.store(false);
    interrupt();
    if (wasOnPause && !m_destroyed)
        emit simulationStopped();
}

void VSimulator::pause()
{
    if (isSimulating())
    {
        m_pauseFlag.store(true);
        interrupt();
    }
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

bool VSimulator::isSimulating() const 
{
    return m_simulatingFlag.load();
}

bool VSimulator::isPaused() const
{
    return m_pauseFlag.load();
}

bool VSimulator::isTimeLimitModeOn() const
{
    return m_timeLimitFlag.load();
}

bool VSimulator::isLifetimeConsidered() const
{
    return m_lifetimeLimitFlag.load();
}

void VSimulator::reset() 
{
    stop();
    resetState();
    m_newDataNotifier.notifyAll();
}

void VSimulator::clear() 
{
    stop();
    resetInfo();
    m_pActiveNodes.reset(new std::vector<VSimNode::ptr>);
    m_pTriangles.reset(new std::vector<VSimTriangle::ptr>);
}

void VSimulator::resetTriangleColors()
{
    trianglesAction([](const VSimTriangle::ptr& triangle){triangle->reset();});
}

VSimulationParameters VSimulator::getSimulationParameters() const
{
    std::lock_guard<std::mutex> locker(*m_pNodesLock);
    return m_param;
}

void VSimulator::setSimulationParameters(const VSimulationInfo &info,
                                         const VSimulationParameters &param,
                                         bool isPaused,
                                         bool isTimeLimited)
{
    stop();
    {
        std::lock_guard<std::mutex> locker(*m_pNodesLock);
        m_param = param;
    }
    emit resinChanged();
    emit injectionDiameterSet(param.getInjectionDiameter()) ;
    emit vacuumDiameterSet(param.getVacuumDiameter()) ;
    emit temperatureSet(param.getTemperature()) ;
    emit vacuumPressureSet(param.getVacuumPressure()) ;
    emit injectionPressureSet(param.getInjectionPressure()) ;
    emit coefQSet(param.getQ()) ;
    emit coefRSet(param.getR()) ;
    emit coefSSet(param.getQ()) ;
    emit timeLimitSet(param.getTimeLimit()) ;
    {
        std::lock_guard<std::mutex> locker(m_infoLock);
        m_info = info;
    }
    m_pauseFlag.store(isPaused);
    if (isPaused)
    {
        m_simT_timeBeforePause = static_cast<int>(info.realTime * 1000);
        emit simulationPaused();
    }
    setTimeLimitMode(isTimeLimited);
    m_newDataNotifier.notifyAll();
}

void VSimulator::setActiveElements(const VSimNode::const_vector_ptr &nodes,
                                const VSimTriangle::const_vector_ptr &triangles) 
{
    if (!isSimulating())
    {
        std::lock(*m_pNodesLock, *m_pTrianglesLock);
        m_pActiveNodes = nodes;
        m_pTriangles = triangles;
        m_nodesThreadPart = m_pActiveNodes->size() / N_THREADS + 1;
        m_trianglesThreadPart = m_pTriangles->size() / N_THREADS + 1;
        m_pNodesLock->unlock();
        m_pTrianglesLock->unlock();
    }
    else
        throw VSimulatorException();
}

VSimulationInfo VSimulator::getSimulationInfo() const
{
    std::lock_guard<std::mutex> lock(m_infoLock);
    return m_info;
}

int VSimulator::getIterationNumber() const  
{
    std::lock_guard<std::mutex> lock(m_infoLock);
    return m_info.iteration;
}

void VSimulator::waitForNewData() const 
{
    m_newDataNotifier.wait();
}

void VSimulator::cancelWaitingForNewData() const 
{
    m_newDataNotifier.notifyAll();
}

void VSimulator::simulationCycle() 
{
    emit simulationStarted();
    QTime timeMeasurer;
    timeMeasurer.start();
    if (!(m_pauseFlag.load()))
    {
        m_pauseFlag.store(false);
        resetState();
        double avgPerm = getAveragePermeability();
        double avgDist = getAverageCellDistance();
        {
            std::lock_guard<std::mutex> locker(*m_pNodesLock);
            m_param.setAveragePermeability(avgPerm);
            m_param.setAverageCellDistance(avgDist);
            m_param.setNumberOfFullNodes(0);
        }
    }
    m_pVacuumNodes->clear();
    for(auto &node : *m_pActiveNodes)
    {
        if (node->isVacuum())
        {
            m_pVacuumNodes->push_back(node);
        }
    }
    bool processIsFinished = false;
    while(!(m_stopFlag.load()))
    {
        double filledPercent = getFilledPercent();
        double averagePressure = getAveragePressure();
        {
            std::lock_guard<std::mutex> locker(m_infoLock);
            double simTimeDelta = getTimeDelta();
            m_info.simTime += simTimeDelta;
            double oldRealTime = m_info.realTime;
            m_info.realTime = (m_simT_timeBeforePause + timeMeasurer.elapsed()) / 1000.0;
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
        processIsFinished = !moveToNextStep();
        updateColors();
        m_newDataNotifier.notifyAll();
        if (processIsFinished)
            break;
        if (m_pVacuumNodes->size() > 0)
        {
            size_t fullVacuumCounter{0};
            std::lock_guard<std::mutex> nodesLocker(*m_pNodesLock);
            for(auto &vacuumNode: *m_pVacuumNodes)
            {
                fullVacuumCounter += vacuumNode->isFull();
            }
            if (fullVacuumCounter == m_pVacuumNodes->size())
            {
                processIsFinished = true;
                break;
            }
        }
        if (m_lifetimeLimitFlag || m_timeLimitFlag)
        {
            std::lock_guard<std::mutex> infoLocker(m_infoLock);
            std::lock_guard<std::mutex> nodesLocker(*m_pNodesLock);
            if (m_lifetimeLimitFlag && m_info.simTime >= m_param.getLifetime())
            {
                processIsFinished = true;
                break;
            }
            else if (m_timeLimitFlag && m_info.simTime >= m_param.getTimeLimit())
            {
                m_pauseFlag.store(true);
                break;
            }
        }
    }
    m_simulatingFlag.store(false);
    if (processIsFinished)
        m_pauseFlag.store(false);
    if (!m_pauseFlag)
    {
        m_simT_timeBeforePause = 0;
        if (!m_destroyed)
            emit simulationStopped();
    }
    else
    {
        m_simT_timeBeforePause += timeMeasurer.elapsed();
        if (!m_destroyed)
            emit simulationPaused();
    }
    #ifdef DEBUG_MODE
        qInfo() << "The simulation was stopped";
    #endif
}

void VSimulator::resetState()
{
    nodesAction([this](const VSimNode::ptr& node)
    {
        node->setBoundaryPressures(m_param.getInjectionPressure(),
                                   m_param.getVacuumPressure());
        node->reset();
    });
    trianglesAction([](const VSimTriangle::ptr& triangle){triangle->reset();});
    resetInfo();
}

void VSimulator::resetInfo() 
{
    std::lock_guard<std::mutex> lock(m_infoLock);
    std::memset(&m_info, 0, sizeof(m_info));
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
    auto calcFunc = [this](const VSimNode::ptr &node) {calculateNewPressure(node);};
    nodesAction(calcFunc);
}

inline bool VSimulator::moveToNextStep()
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
    {
        std::lock_guard<std::mutex> locker(*m_pNodesLock);
        m_param.setNumberOfFullNodes(fullNodesCount);
    }
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
    for(auto &node : *m_pActiveNodes)
    {
        const VSimNode::layered_neighbours_t &neighbours = node->getNeighbours();
        for (uint layer = 0; layer < VSimNode::LAYERS_NUMBER; ++layer)
        {
            for(auto &neighbour : neighbours[layer])
            {
                distance += node->getDistance(neighbour.second);
                ++counter;
            }
        }
    }
    if (counter > 0)
        distance /= counter;
    return distance;
}

inline double VSimulator::getTimeDelta() const
{
    std::lock_guard<std::mutex> locker(*m_pNodesLock);
    if (m_pActiveNodes->size() > 0)
    {
        double n = m_param.getViscosity();                             //[N*s/m2]
        double _l = m_param.getAverageCellDistance();                   //[m]
        double l_typ = (sqrt((double)m_param.getNumberOfFullNodes())*_l);   //[m]
        double _K = m_param.getAveragePermeability();                   //[m^2]
        double p_inj = m_param.getInjectionPressure();          //[N/m2]
        double p_vac = m_param.getVacuumPressure();                    //[N/m2]

        double time = n*l_typ*_l/(_K*(p_inj-p_vac));
        return time;
    }
    else
        return 0;
}

void VSimulator::calculateNewPressure(const VSimNode::ptr &node)
{
    double m = node->getNeighboursNumber();
    if(!node->isInjection() && m > 0)
    {
        double _K = m_param.getAveragePermeability();
        double K = node->getPermeability();
        double phi = node->getPorosity();
        double d = node->getCavityHeight();
        double _l = m_param.getAverageCellDistance();
        double q = m_param.getQ();
        double r = m_param.getR();
        double s = m_param.getS();
        double p_t = node->getPressure();

        double brace0 = pow(K/_K,q);

        double sum = 0;
        double highestNeighborPressure = 0;

        double den_brace1 = d*phi;
        if (K > 0 && _K > 0 && den_brace1 > 0 )
        {
            const VSimNode::layered_neighbours_t &neighbours = node->getNeighbours();
            for (uint layer = 0; layer < VSimNode::LAYERS_NUMBER; ++layer)
            {
                for(auto &it: neighbours[layer])
                {
                    double distance = it.first;
                    if (distance > 0)
                    {
                        const VSimNode* neighbor = it.second;
                        double K_i = neighbor->getPermeability();
                        double d_i = neighbor->getCavityHeight();
                        double phi_i = neighbor->getPorosity();
                        double brace1 = pow(((d_i*phi_i)/den_brace1),r);
                        double brace2 = pow(_l/distance,s);
                        double p_it = neighbor->getPressure();
                        double brace3 = p_it-p_t;
                        double brace4 = K_i / K;
                        sum += brace1 * brace2 * brace3 * brace4;
                        if(p_it > highestNeighborPressure)
                            highestNeighborPressure = p_it;
                    }
                    else
                        --m;
                }
            }
        }
        if (m > 0)
        {
            double newPressure = p_t+(brace0/m)*sum;
            if(newPressure < p_t)
                newPressure = p_t;
            if(newPressure > m_param.getInjectionPressure())
                newPressure = m_param.getInjectionPressure();

            if(newPressure > highestNeighborPressure)
                    newPressure = highestNeighborPressure;
            if(node->isVacuum() && newPressure > m_param.getVacuumPressure())
                    newPressure = m_param.getVacuumPressure();
            node->setNewPressure(newPressure);
        }
    }
    else
    {
        node->setNewPressure(node->getPressure());
    }
}



void VSimulator::setResin(const VResin& resin)
{
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        m_param.setResin(resin);
    }
    emit resinChanged();
}

void VSimulator::setInjectionDiameter(float diameter)
{
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        m_param.setInjectionDiameter(diameter);
    }
    emit injectionDiameterSet(diameter);
}

void VSimulator::setVacuumDiameter(float diameter)
{
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        m_param.setVacuumDiameter(diameter);
    }
    emit vacuumDiameterSet(diameter);
}

void VSimulator::setTemperature(double temperature)
{
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        m_param.setTemperature(temperature);
    }
    emit temperatureSet(temperature);
}

void VSimulator::setVacuumPressure(double pressure)
{
    std::lock(*m_pNodesLock, *m_pTrianglesLock);
    m_param.setVacuumPressure(pressure);
    m_pNodesLock->unlock();
    m_pTrianglesLock->unlock();
    emit vacuumPressureSet(pressure);
}

void VSimulator::setInjectionPressure(double pressure)
{
    std::lock(*m_pNodesLock, *m_pTrianglesLock);
    m_param.setInjectionPressure(pressure);
    m_pNodesLock->unlock();
    m_pTrianglesLock->unlock();
    emit injectionPressureSet(pressure);
}

void VSimulator::setQ(double q)
{
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        m_param.setQ(q);
    }
    emit coefQSet(q);
}

void VSimulator::setR(double r)
{
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        m_param.setR(r);
    }
    emit coefRSet(r);
}

void VSimulator::setS(double s)
{
    {
        std::lock_guard<std::mutex> lock(*m_pNodesLock);
        m_param.setS(s);
    }
    emit coefSSet(s);
}

void VSimulator::setTimeLimit(double limit)
{
    {
        std::lock_guard<std::mutex> locker(*m_pNodesLock);
        m_param.setTimeLimit(limit);
    }
    emit timeLimitSet(limit);
}

void VSimulator::setTimeLimitMode(bool on)
{
    m_timeLimitFlag.store(on);
    emit timeLimitModeSwitched(on);
}

void VSimulator::considerLifetime(bool on)
{
    m_lifetimeLimitFlag.store(on);
    emit lifetimeConsiderationSwitched(on);
}
