/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <functional>
//#include <shared_mutex> //TODO
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

/**
 * Update an information about active nodes and triangles (m_activeNodes, m_triangles)
 * @param layers
 */
void VSimulator::setActiveElements(VSimNode::const_vector_ptr nodes,
                                VSimTriangle::const_vector_ptr triangles) noexcept(false)
//TODO make shared ptrs instead of m_activeNodes, m_triangles and arguments of this function
{
    if (!isSimulating())
    {
        //COPY shared pointers to given nodes and triangles
        m_pActiveNodes = nodes;
        m_pTriangles = triangles;
        m_nodesThreadPart = m_pActiveNodes->size() / N_THREADS + 1;
        m_trianglesThreadPart = m_pTriangles->size() / N_THREADS + 1;
        m_pParam->averagePermeability = calcAveragePermeability();
        m_pParam->averageCellDistance = calcAverageCellDistance();
        m_pParam->numberOfNodes = m_pActiveNodes->size();
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
    //bool madeChangesInCycle;
    std::atomic<bool> madeChangesInCycle;
    //std::shared_mutex madeChangesLock;  //TODO
    auto calcFunc = [](const VSimNode::ptr& node){node->calculate();};
    auto commitFunc = [&madeChangesInCycle/*, &madeChangesLock TODO*/](const VSimNode::ptr& node)
    {
        bool madeChanges = node->commit();
        if(madeChanges && !madeChangesInCycle.load())
            madeChangesInCycle.store(true);
        /*
        if (madeChanges)
        {
            bool needToRewrite;
            {
                std::shared_lock<std::shared_mutex> lock(madeChangesLock);
                needToRewrite = !madeChangesInCycle;
            }
            if(needToRewrite)
            {
                std::unique_lock<std::shared_mutex> lock(madeChangesLock);
                madeChangesInCycle = true;
            }
            //TODO: measure speed
        }
            */
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
    std::lock_guard lock(*m_pNodesLock);
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
    std::lock_guard lock(*m_pTrianglesLock);
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
    double n = m_pParam->viscosity;                             //[N*s/m2]
    double _l = m_pParam->averageCellDistance;                   //[m]
    double l_typ = (sqrt((double)m_pParam->numberOfNodes)*_l);   //[m]
    double _K = m_pParam->averagePermeability;                   //[m^2]
    double p_inj = m_pParam->injectionPressure;          //[N/m2]
    double p_vac = m_pParam->vacuumPressure;                    //[N/m2]

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
    std::vector<VSimNode::const_ptr> neighbours;
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
    m_pParam->injectionDiameter = diameter;
}
void VSimulator::setVacuumDiameter(double diameter) noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    m_pParam->vacuumDiameter = diameter;
}
void VSimulator::setViscosity(double viscosity) noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    m_pParam->viscosity = viscosity;
}
void VSimulator::setVacuumPressure(double pressure) noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    std::lock_guard<std::mutex> lockT(*m_pTrianglesLock);
    m_pParam->vacuumPressure = pressure;
}
void VSimulator::setInjectionPressure(double pressure) noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    std::lock_guard<std::mutex> lockT(*m_pTrianglesLock);
    m_pParam->injectionPressure = pressure;
}
void VSimulator::setQ(double q) noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    m_pParam->q = q;
}
void VSimulator::setR(double r) noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    m_pParam->r = r;
}
void VSimulator::setS(double s) noexcept
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    m_pParam->s = s;
}
