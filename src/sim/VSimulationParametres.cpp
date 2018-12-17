/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <cmath>

#include "VSimulationParametres.h"

/**
 * VSimulationParametres implementation
 */

const double VSimulationParametres::DEFAULT_TEMPERATURE = 25;
const double VSimulationParametres::KELVINS_IN_0_CELSIUS = 273.15;
const double VSimulationParametres::MIN_PRESSURE = 1;

VSimulationParametres::VSimulationParametres():
    m_numberOfFullNodes(0),
    averagePermeabilityAccessFlag(false),
    averageCellDistanceAccessFlag(false)
{}

double VSimulationParametres::getInjectionDiameter() const 
{
    return m_injectionDiameter;
}

void VSimulationParametres::setInjectionDiameter(double diameter) 
{
    if (diameter < 0)
        diameter = 0;
    m_injectionDiameter = diameter;
}

double VSimulationParametres::getVacuumDiameter() const 
{
    return m_vacuumDiameter;
}
void VSimulationParametres::setVacuumDiameter(double diameter) 
{
    if (diameter < 0)
        diameter = 0;
    m_vacuumDiameter = diameter;
}

double VSimulationParametres::getViscosity() const 
{
    return m_viscosity;
}

double VSimulationParametres::getDefaultViscosity() const 
{
    return m_resin.defaultViscosity;
}

double VSimulationParametres::getTemperature() const 
{
    return m_temperature;
}
void VSimulationParametres::setTemperature(double temperature) 
{
    if (temperature <= - KELVINS_IN_0_CELSIUS)
        temperature = - KELVINS_IN_0_CELSIUS + 1;
    m_temperature = temperature;
    m_viscosity = calculateViscosity();
}

double VSimulationParametres::getTempcoef() const 
{
    return m_resin.tempcoef;
}

void VSimulationParametres::setResin(const VResin &resin)
{
    m_resin = resin;
    calculateViscosity();
}

const VResin& VSimulationParametres::getResin() const
{
    return m_resin;
}

double VSimulationParametres::getInjectionPressure() const 
{
    return m_injectionPressure;
}
void VSimulationParametres::setInjectionPressure(double pressure) 
{
    if (pressure < MIN_PRESSURE)
        pressure = MIN_PRESSURE;
    m_injectionPressure = pressure;
}

double VSimulationParametres::getVacuumPressure() const 
{
    return m_vacuumPressure;
}
void VSimulationParametres::setVacuumPressure(double pressure) 
{
    if (pressure < MIN_PRESSURE)
        pressure = MIN_PRESSURE;
    m_vacuumPressure = pressure;
}

double VSimulationParametres::getQ() const 
{
    return m_q;
}
void VSimulationParametres::setQ(double q) 
{
    if (q < 1)
        q = 1;
    m_q = q;
}

double VSimulationParametres::getR() const 
{
    return m_r;
}
void VSimulationParametres::setR(double r) 
{
    if (r < 1)
        r = 1;
    m_r = r;
}

double VSimulationParametres::getS() const 
{
    return m_s;
}
void VSimulationParametres::setS(double s) 
{
    if (s < 1)
        s = 1;
    m_s = s;
}

double VSimulationParametres::getAveragePermeability() const 
{
    double averagePermeability;
    if(!averagePermeabilityAccessFlag)
    {
        std::lock_guard<std::mutex> locker(averagePermeabilityLock);
        averagePermeability = m_averagePermeability;
    }
    else
        averagePermeability = m_averagePermeability;
    return averagePermeability;
}

void VSimulationParametres::setAveragePermeability(double averagePermeability) 
{
    if (averagePermeability < 0)
        averagePermeability = 0;
    averagePermeabilityAccessFlag.store(true);
    {
        std::lock_guard<std::mutex> locker(averagePermeabilityLock);
        m_averagePermeability = averagePermeability;
    }
    averagePermeabilityAccessFlag.store(false);
}

double VSimulationParametres::getAverageCellDistance() const 
{
    double averageCellDistance;
    if(!averageCellDistanceAccessFlag)
    {
        std::lock_guard<std::mutex> locker(averageCellDistanceLock);
        averageCellDistance = m_averageCellDistance;
    }
    else
        averageCellDistance = m_averageCellDistance;
    return averageCellDistance;
}

void VSimulationParametres::setAverageCellDistance(double averageCellDistance) 
{
    if (averageCellDistance < 0)
        averageCellDistance = 0;
    averageCellDistanceAccessFlag.store(true);
    {
        std::lock_guard<std::mutex> locker(averageCellDistanceLock);
        m_averageCellDistance = averageCellDistance;
    }
    averageCellDistanceAccessFlag.store(false);
}

int VSimulationParametres::getNumberOfFullNodes() const
{
    return m_numberOfFullNodes;
}

void VSimulationParametres::setNumberOfFullNodes(int numberOfNodes)
{
    m_numberOfFullNodes = numberOfNodes;
}

double VSimulationParametres::calculateViscosity() const 
{
    double A = m_resin.tempcoef;
    double mu0 = m_resin.defaultViscosity;
    double T = m_temperature - KELVINS_IN_0_CELSIUS;
    double T0 = DEFAULT_TEMPERATURE - KELVINS_IN_0_CELSIUS;
    if (T != T0)
    {
        double mu = pow(A, 1 - T0/T) * pow(mu0, T0/T);
        return mu;
    }
    else
        return mu0;
}

