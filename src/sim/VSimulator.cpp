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
    m_trianglesThreadPart(1)
{
    m_newDataLock.lock();
    m_calculationThreads.reserve(N_THREADS);
}
/**
 * Destructor
 */
VSimulator::~VSimulator()
{
    stopSimulation();
}

/**
 * Start the simulation thread
 */
void VSimulator::startSimulation() noexcept
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
void VSimulator::stopSimulation() noexcept
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
    stopSimulation();
    nodesAction([](VSimNode::ptr& node){node->reset();});
    trianglesAction([](VSimTriangle::ptr& triangle){triangle->reset();});
    m_newDataLock.unlock();
}

/**
 * Update an information about active nodes and triangles (m_activeNodes, m_triangles)
 * @param layers
 */
void VSimulator::setActiveNodes(std::vector<VLayer::ptr> &layers) noexcept(false)
{
    if (!isSimulating())
    {
        //TODO
        m_nodesThreadPart = m_activeNodes.size() / N_THREADS + 1;
        m_trianglesThreadPart = m_triangles.size() / N_THREADS + 1;
    }
    else
        throw VSimulatorException();
}

/**
 * Get the information about the current state of the simulation
 * @param info: output information about the current state of the simulation
 */
void VSimulator::getSimulationInfo(VSimulationInfo &info) const noexcept
{
    //TODO
}

/**
 * Create Graphic elements for VGraphicsView: nodes and triangles, connected to simulation elements
 * @param nodes_p
 * @param triangles_p
 */
void VSimulator::createGraphicsElements(std::vector<VGraphicsNode *> &nodes_p,
                                     std::vector<VGraphicsTriangle *> &triangles_p) const noexcept(false)
{
    if (!isSimulating())
    {
        createGraphicsElementsCore(nodes_p, m_activeNodes);
        createGraphicsElementsCore(triangles_p, m_triangles);
    }
    else
        throw VSimulatorException();
}

template<typename T1, typename T2>
inline void VSimulator::createGraphicsElementsCore(std::vector<T1 *> &gaphics,
                                            const std::vector< std::shared_ptr<T2> > &sim) const noexcept
{
    gaphics.clear();
    gaphics.reserve(sim.size());
    for (auto &simElem : sim)
    {
        if (simElem->isVisible())
            gaphics.push_back(new T1(simElem));
    }
}

/**
 * Get number of current iteration
 * @return int
 */
int VSimulator::getIterationNumber() const  noexcept
{
    return m_iteration.load();
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
    m_iteration.store(0);
    //bool madeChangesInCycle;
    std::atomic<bool> madeChangesInCycle;
    std::shared_mutex madeChangesLock;
    auto calcFunc = [](VSimNode::ptr& node){node->calculate();};
    auto commitFunc = [&madeChangesInCycle, &madeChangesLock](VSimNode::ptr& node)
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
    auto updateColorsFunc = [](VSimTriangle::ptr& triangle){triangle->updateColor();};
    while(!m_stopFlag.load())
    {
        m_iteration++;
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
                    for(size_t j = batchStart; j < batchStop && j < m_activeNodes.size(); ++j )
                        func(m_activeNodes[j]);
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
                    for(size_t j = batchStart; j < batchStop && j < m_triangles.size(); ++j )
                        func(m_triangles[j]);
                };
        m_calculationThreads.emplace_back(prc);
        batchStart += m_trianglesThreadPart;
    }
    for(auto &thread : m_calculationThreads)
        thread.join();
}

/*
inline double VSimulator::timeDelta()
{
    double n = viscosity();                             //[N*s/m2]
    double _l = medianCellDistance();                   //[m]
    double l_typ = (sqrt((double)m_numNodesFull)*_l);   //[m]
    double _K = medianPermeability();                   //[m^2]
    double p_inj = highestInjectionPressure();          //[N/m2]
    double p_vac = vacuumPressure();                    //[N/m2]

    double time = n*l_typ*_l/(_K*(p_inj-p_vac));
    return time;
}
*/
