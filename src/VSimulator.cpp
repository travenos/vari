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
void VSimulator::VSimulator() {

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
bool VSimulator::isSimulating() {
    return false;
}

void VSimulator::reset() {

}

/**
 * Update an information about active nodes and triangles (m_activeNodes, m_triangles)
 * @param layers
 */
void VSimulator::setActiveNodes(std::vector<VLayer>& layers) {

}

void VSimulator::getSimulationInfo() {

}

void VSimulator::createGraphicsNodes() {

}

/**
 * @return int
 */
int VSimulator::getIterationNumber() {
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