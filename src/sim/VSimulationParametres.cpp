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
    return m_defaultViscosity;
}
void VSimulationParametres::setDefaultViscosity(double viscosity) 
{
    m_defaultViscosity = viscosity;
    m_viscosity = calculateViscosity();
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
    return m_tempcoef;
}

void VSimulationParametres::setTempcoef(double tempcoef) 
{
    if (tempcoef < 0)
        tempcoef = 0;
    m_tempcoef = tempcoef;
    m_viscosity = calculateViscosity();
}

double VSimulationParametres::getInjectionPressure() const 
{
    return m_injectionPressure;
}
void VSimulationParametres::setInjectionPressure(double pressure) 
{
    if (pressure < 0)
        pressure = 0;
    m_injectionPressure = pressure;
}

double VSimulationParametres::getVacuumPressure() const 
{
    return m_vacuumPressure;
}
void VSimulationParametres::setVacuumPressure(double pressure) 
{
    if (pressure < 0)
        pressure = 0;
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
    return m_averagePermeability;
}
void VSimulationParametres::setAveragePermeability(double averagePermeability) 
{
    if (averagePermeability < 0)
        averagePermeability = 0;
    m_averagePermeability = averagePermeability;
}

double VSimulationParametres::getAverageCellDistance() const 
{
    return m_averageCellDistance;
}
void VSimulationParametres::setAverageCellDistance(double averageCellDistance) 
{
    if (averageCellDistance < 0)
        averageCellDistance = 0;
    m_averageCellDistance = averageCellDistance;
}

size_t VSimulationParametres::getNumberOfNodes() const
{
    return m_numberOfNodes;
}
void VSimulationParametres::setNumberOfNodes(size_t numberOfNodes)
{
    m_numberOfNodes = numberOfNodes;
}

double VSimulationParametres::calculateViscosity() const 
{
    double A = m_tempcoef;
    double mu0 = m_defaultViscosity;
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

