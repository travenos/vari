/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <cmath>
#include <algorithm>

#include "VSimulationParameters.h"

/**
 * VSimulationParameters implementation
 */

const double VSimulationParameters::DEFAULT_TEMPERATURE = 25;
const double VSimulationParameters::KELVINS_IN_0_CELSIUS = 273.15;
const double VSimulationParameters::MIN_PRESSURE = 1;

double VSimulationParameters::getInjectionDiameter() const
{
    return m_injectionDiameter;
}

void VSimulationParameters::setInjectionDiameter(double diameter)
{
    m_injectionDiameter = std::max(diameter, 0.0);
}

double VSimulationParameters::getVacuumDiameter() const
{
    return m_vacuumDiameter;
}
void VSimulationParameters::setVacuumDiameter(double diameter)
{
    m_vacuumDiameter = std::max(diameter, 0.0);
}

double VSimulationParameters::getViscosity() const
{
    return m_viscosity;
}

double VSimulationParameters::getDefaultViscosity() const
{
    return m_resin.defaultViscosity;
}

double VSimulationParameters::getTemperature() const
{
    return m_temperature;
}
void VSimulationParameters::setTemperature(double temperature)
{
    if (temperature <= - KELVINS_IN_0_CELSIUS)
        temperature = - KELVINS_IN_0_CELSIUS + 1;
    m_temperature = temperature;
    m_viscosity = calculateViscosity();
}

double VSimulationParameters::getTempcoef() const
{
    return m_resin.tempcoef;
}

void VSimulationParameters::setResin(const VResin &resin)
{
    m_resin = resin;
    calculateViscosity();
}

VResin VSimulationParameters::getResin() const
{
    return m_resin;
}

double VSimulationParameters::getInjectionPressure() const
{
    return m_injectionPressure;
}
void VSimulationParameters::setInjectionPressure(double pressure)
{
    m_injectionPressure = std::max(pressure, MIN_PRESSURE);
}

double VSimulationParameters::getVacuumPressure() const
{
    return m_vacuumPressure;
}
void VSimulationParameters::setVacuumPressure(double pressure)
{
    m_vacuumPressure = std::max(pressure, MIN_PRESSURE);
}

double VSimulationParameters::getQ() const
{
    return m_q;
}
void VSimulationParameters::setQ(double q)
{
    m_q = std::max(q, 0.0);
}

double VSimulationParameters::getR() const
{
    return m_r;
}
void VSimulationParameters::setR(double r)
{
    m_r = std::max(r, 0.0);
}

double VSimulationParameters::getS() const
{
    return m_s;
}
void VSimulationParameters::setS(double s)
{
    m_s = std::max(s, 0.0);
}

double VSimulationParameters::getAveragePermeability() const
{
    return m_averagePermeability;
}

void VSimulationParameters::setAveragePermeability(double averagePermeability)
{
    m_averagePermeability = averagePermeability;
}

double VSimulationParameters::getAverageCellDistance() const
{
    return m_averageCellDistance;
}

void VSimulationParameters::setAverageCellDistance(double averageCellDistance)
{
    m_averageCellDistance = averageCellDistance;
}

int VSimulationParameters::getNumberOfFullNodes() const
{
    return m_numberOfFullNodes;
}

void VSimulationParameters::setNumberOfFullNodes(int numberOfNodes)
{
    m_numberOfFullNodes = numberOfNodes;
}

double VSimulationParameters::getTimeLimit() const
{
    return m_timeLimit;
}

void VSimulationParameters::setTimeLimit(double limit)
{
    m_timeLimit = std::max(limit, 0.0);
}

double VSimulationParameters::calculateViscosity() const
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
