/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VSimulator.h"

/**
 * VSimulator implementation
 */


/**
 * Default constructor
 */
VSimulator::VSimulator() {
    m_newDataLock.lock();
}

/**
 * Start the simulation thread
 */
void VSimulator::startSimulation() noexcept {

}

/**
 * Finish the simulation using a flag m_stopFlag
 */
void VSimulator::stopSimulation() noexcept {

}

/**
 * Check if simulation thread is currently active
 * @return bool
 */
bool VSimulator::isSimulating() const noexcept {
    return m_simulatingFlag.load();
}

void VSimulator::reset() noexcept {

}

/**
 * Update an information about active nodes and triangles (m_activeNodes, m_triangles)
 * @param layers
 */
void VSimulator::setActiveNodes(std::vector<VLayer::ptr> &layers) noexcept(false) {

}

void VSimulator::getSimulationInfo(VSimulationInfo &info) const noexcept {

}

void VSimulator::createGraphicsNodes() noexcept(false) {

}

/**
 * @return int
 */
int VSimulator::getIterationNumber() const  noexcept {
    return 0;
}

void VSimulator::waitForNewData() const noexcept
{
    m_newDataLock.lock();
}

void VSimulator::cancelWaitingForNewData() const noexcept
{
    m_newDataLock.unlock();
}

/**
 * A function, which is being executed in the simulation thread
 */
void VSimulator::simulationCycle() noexcept {
    m_newDataLock.unlock();
}

void VSimulator::calculateNodes() noexcept {

}

void VSimulator::commitNodes() noexcept {

}

void VSimulator::setTriangleColors() noexcept {

}
