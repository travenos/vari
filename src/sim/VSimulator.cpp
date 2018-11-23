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

}

/**
 * Start the simulation thread
 */
void VSimulator::startSimulation() {

}

/**
 * Finish the simulation using a flag m_stopFlag
 */
void VSimulator::stopSimulation() {

}

/**
 * Check if simulation thread is currently active
 * @return bool
 */
bool VSimulator::isSimulating() const {
    return m_simulatingFlag.load();
}

void VSimulator::reset() {

}

/**
 * Update an information about active nodes and triangles (m_activeNodes, m_triangles)
 * @param layers
 */
void VSimulator::setActiveNodes(std::vector<VLayer::::ptr> &layers) {

}

void VSimulator::getSimulationInfo(VSimulationInfo &info) const {

}

void VSimulator::createGraphicsNodes() {

}

/**
 * @return int
 */
int VSimulator::getIterationNumber() const {
    return 0;
}

/**
 * A function, which is being executed in the simulation thread
 */
void VSimulator::simulationCycle() {

}

void VSimulator::calculateNodes() {

}

void VSimulator::commitNodes() {

}

void VSimulator::setTriangleColors() {

}
