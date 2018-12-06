/**
 * Project VARI
 * @author Alexey Barashkov
 */
#include <functional>
#include "VSimulator.h"
#include "VExceptions.h"

/**
 * VSimulator implementation
 */

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
    m_pParam(new VSimulationParametres)
{
    m_newDataLock.lock();
    m_calculationThreads.reserve(N_THREADS);
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
void VSimulator::start() noexcept
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
void VSimulator::stop() noexcept
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
bool VSimulator::isSimulating() const noexcept
{
    return m_simulatingFlag.load();
}

/**
 * Stop the simulation if it is active and set the default state for all nodes
 */
void VSimulator::reset() noexcept
{
    stop();
    nodesAction([](const VSimNode::ptr& node){node->reset();});
    trianglesAction([](const VSimTriangle::ptr& triangle){triangle->reset();});
    resetInfo();
    m_newDataLock.unlock();
}

void VSimulator::clear() noexcept
{
    stop();
    resetInfo();
    m_pActiveNodes.reset(new std::vector<VSimNode::ptr>);
    m_pTriangles.reset(new std::vector<VSimTriangle::ptr>);
}

VSimulationParametres::const_ptr VSimulator::getSimulationParametres() const noexcept
{
    return m_pParam;
}

/**
 * Update an information about active nodes and triangles (m_activeNodes, m_triangles)
 * @param layers
 */
void VSimulator::setActiveElements(const VSimNode::const_vector_ptr &nodes,
                                const VSimTriangle::const_vector_ptr &triangles) noexcept(false)
{
    if (!isSimulating())
    {
        m_pActiveNodes = nodes;
        m_pTriangles = triangles;
        m_nodesThreadPart = m_pActiveNodes->size() / N_THREADS + 1;
        m_trianglesThreadPart = m_pTriangles->size() / N_THREADS + 1;
        m_pParam->setAveragePermeability(calcAveragePermeability());
        m_pParam->setAverageCellDistance(calcAverageCellDistance());
        m_pParam->setNumberOfNodes(m_pActiveNodes->size());
    }
    else
        throw VSimulatorException();
}

/**
 * Get the information about the current state of the simulation
 * @param info: output information about the current state of the simulation
 */
VSimulator::VSimulationInfo VSimulator::getSimulationInfo() const noexcept
{
    std::lock_guard<std::mutex> lock(m_infoLock);
    return m_info;
}

/**
 * Get number of current iteration
 * @return int
 */
int VSimulator::getIterationNumber() const  noexcept
{
    std::lock_guard<std::mutex> lock(m_infoLock);
    return m_info.iteration;
}
/**
 * Wait until some nodes state is changed
 */
void VSimulator::waitForNewData() const noexcept
{
    m_newDataLock.lock();
}
/**
 * Make waiting thread stop waiting and make it think that the simulation state has changed
 */
void VSimulator::cancelWaitingForNewData() const noexcept
{
    m_newDataLock.unlock();
}

/**
 * A function, which is being executed in the simulation thread
 */
void VSimulator::simulationCycle() noexcept
{
    resetInfo();
    std::atomic<bool> madeChangesInCycle;
    auto calcFunc = [](const VSimNode::ptr& node){node->calculate();};
    auto commitFunc = [&madeChangesInCycle](const VSimNode::ptr& node)
    {
        bool madeChanges = node->commit();
        if(madeChanges && !madeChangesInCycle.load())
            madeChangesInCycle.store(true);
    };
    auto updateColorsFunc = [](const VSimTriangle::ptr& triangle){triangle->updateColor();};
    while(!m_stopFlag.load())
    {
        {
            std::lock_guard<std::mutex> lock(m_infoLock);
            m_info.time += timeDelta();
            ++(m_info.iteration);
        }
        madeChangesInCycle = false;
        nodesAction(calcFunc);
        nodesAction(commitFunc);
        trianglesAction(updateColorsFunc);
        m_newDataLock.unlock();
        if (!madeChangesInCycle)
            break;
    }
    m_simulatingFlag.store(false);
}

void VSimulator::resetInfo() noexcept
{
    std::lock_guard<std::mutex> lock(m_infoLock);
    m_info.averagePressure = 0;
    m_info.filledPercent = 0;
    m_info.time = 0;
    m_info.iteration = 0;
}

/**
 * Perform an action over all nodes
 * @param func Describes an action for node
 */
template <typename Callable>
inline void VSimulator::nodesAction(Callable&& func) noexcept
{
    m_calculationThreads.clear();
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
        m_calculationThreads.emplace_back(prc);
        batchStart += m_nodesThreadPart;
    }
    for(auto &thread : m_calculationThreads)
        thread.join();
}

/**
 * Perform an action over all triangles
 * @param func Describes an action for triangle
 */
template <typename Callable>
inline void VSimulator::trianglesAction(Callable&& func) noexcept
{
    m_calculationThreads.clear();
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
        m_calculationThreads.emplace_back(prc);
        batchStart += m_trianglesThreadPart;
    }
    for(auto &thread : m_calculationThreads)
        thread.join();
}

inline double VSimulator::timeDelta() const noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    double n = m_pParam->getViscosity();                             //[N*s/m2]
    double _l = m_pParam->getAverageCellDistance();                   //[m]
    double l_typ = (sqrt((double)m_pParam->getNumberOfNodes())*_l);   //[m]
    double _K = m_pParam->getAveragePermeability();                   //[m^2]
    double p_inj = m_pParam->getInjectionPressure();          //[N/m2]
    double p_vac = m_pParam->getVacuumPressure();                    //[N/m2]

    double time = n*l_typ*_l/(_K*(p_inj-p_vac));
    return time;
}

inline double VSimulator::calcAveragePermeability() const noexcept
{
    double permeability = 0;
    for(auto &node : *m_pActiveNodes)
        permeability += node->getPermeability();
    if (m_pActiveNodes->size() > 0)
        permeability /= m_pActiveNodes->size();
    return permeability;
}

inline double VSimulator::calcAverageCellDistance() const noexcept
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

void VSimulator::setInjectionDiameter(double diameter) noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    m_pParam->setInjectionDiameter(diameter);
}
void VSimulator::setVacuumDiameter(double diameter) noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    m_pParam->setVacuumDiameter(diameter);
}
void VSimulator::setDefaultViscosity(double defaultViscosity) noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    m_pParam->setDefaultViscosity(defaultViscosity);
}
void VSimulator::setTempcoef(double tempcoef) noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    m_pParam->setTempcoef(tempcoef);
}
void VSimulator::setTemperature(double temperature) noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    m_pParam->setTemperature(temperature);
}
void VSimulator::setVacuumPressure(double pressure) noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    std::lock_guard<std::mutex> lockT(*m_pTrianglesLock);
    m_pParam->setVacuumPressure(pressure);
}
void VSimulator::setInjectionPressure(double pressure) noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    std::lock_guard<std::mutex> lockT(*m_pTrianglesLock);
    m_pParam->setInjectionPressure(pressure);
}
void VSimulator::setQ(double q) noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    m_pParam->setQ(q);
}
void VSimulator::setR(double r) noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    m_pParam->setR(r);
}
void VSimulator::setS(double s) noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    m_pParam->setS(s);
}
