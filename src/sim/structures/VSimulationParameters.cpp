/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <cmath>

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
    if (diameter < 0)
        diameter = 0;
    m_injectionDiameter = diameter;
}

double VSimulationParameters::getVacuumDiameter() const
{
    return m_vacuumDiameter;
}
void VSimulationParameters::setVacuumDiameter(double diameter)
{
    if (diameter < 0)
        diameter = 0;
    m_vacuumDiameter = diameter;
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
    if (pressure < MIN_PRESSURE)
        pressure = MIN_PRESSURE;
    m_injectionPressure = pressure;
}

double VSimulationParameters::getVacuumPressure() const
{
    return m_vacuumPressure;
}
void VSimulationParameters::setVacuumPressure(double pressure)
{
    if (pressure < MIN_PRESSURE)
        pressure = MIN_PRESSURE;
    m_vacuumPressure = pressure;
}

double VSimulationParameters::getQ() const
{
    return m_q;
}
void VSimulationParameters::setQ(double q)
{
    if (q < 1)
        q = 1;
    m_q = q;
}

double VSimulationParameters::getR() const
{
    return m_r;
}
void VSimulationParameters::setR(double r)
{
    if (r < 1)
        r = 1;
    m_r = r;
}

double VSimulationParameters::getS() const
{
    return m_s;
}
void VSimulationParameters::setS(double s)
{
    if (s < 1)
        s = 1;
    m_s = s;
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
    if (limit < 0)
        limit = 0;
    m_timeLimit = limit;
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
