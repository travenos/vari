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

const double VSimulationParameters::DEFAULT_TEMPERATURE{25};
const double VSimulationParameters::KELVINS_IN_0_CELSIUS{273.15};
const double VSimulationParameters::UNIVERSAL_GAS_CONSTANT{8.314};
const double VSimulationParameters::MIN_PRESSURE{1};

void VSimulationParameters::setInjectionDiameter(float diameter)
{
    m_injectionDiameter = std::max(diameter, 0.0f);
}

void VSimulationParameters::setVacuumDiameter(float diameter)
{
    m_vacuumDiameter = std::max(diameter, 0.0f);
}

void VSimulationParameters::setTemperature(double temperature)
{
    if (temperature <= - KELVINS_IN_0_CELSIUS)
        temperature = - KELVINS_IN_0_CELSIUS + 1;
    m_temperature = temperature;
    m_viscosity = calculateViscosity();
    m_lifetime = calculateLifetime();
}

void VSimulationParameters::setResin(const VResin &resin)
{
    m_resin = resin;
    m_viscosity = calculateViscosity();
    m_lifetime = calculateLifetime();
}

void VSimulationParameters::setInjectionPressure(double pressure)
{
    m_injectionPressure = std::max(pressure, MIN_PRESSURE);
}

void VSimulationParameters::setVacuumPressure(double pressure)
{
    m_vacuumPressure = std::max(pressure, MIN_PRESSURE);
}

void VSimulationParameters::setQ(double q)
{
    m_q = std::max(q, 0.0);
}

void VSimulationParameters::setR(double r)
{
    m_r = std::max(r, 0.0);
}

void VSimulationParameters::setS(double s)
{
    m_s = std::max(s, 0.0);
}

void VSimulationParameters::setAveragePermeability(double averagePermeability)
{
    m_averagePermeability = averagePermeability;
}

void VSimulationParameters::setAverageCellDistance(double averageCellDistance)
{
    m_averageCellDistance = averageCellDistance;
    m_halfAverageCellDistance = averageCellDistance / 2;
}

void VSimulationParameters::setNumberOfFullNodes(int numberOfNodes)
{
    m_numberOfFullNodes = numberOfNodes;
}

void VSimulationParameters::setTimeLimit(double limit)
{
    m_timeLimit = std::max(limit, 0.0);
}

double VSimulationParameters::calculateViscosity() const
{
    double R{UNIVERSAL_GAS_CONSTANT};
    double mu0{m_resin.defaultViscosity};
    double T{m_temperature - KELVINS_IN_0_CELSIUS};
    double T0{DEFAULT_TEMPERATURE - KELVINS_IN_0_CELSIUS};
    double Ea{m_resin.viscTempcoef};
    if (T != T0)
    {
        double mu{mu0 * exp(Ea / R * (1 / T - 1/ T0))};
        return mu;
    }
    else
        return mu0;
}

double VSimulationParameters::calculateLifetime() const
{
    double R{UNIVERSAL_GAS_CONSTANT};
    double t0{m_resin.defaultLifetime};
    double T{m_temperature - KELVINS_IN_0_CELSIUS};
    double T0{DEFAULT_TEMPERATURE - KELVINS_IN_0_CELSIUS};
    double Ea{m_resin.lifetimeTempcoef};
    if (T != T0)
    {
        double t{t0 * exp(Ea / R * (1 / T - 1/ T0))};
        return t;
    }
    else
        return t0;
}
