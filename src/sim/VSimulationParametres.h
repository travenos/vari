/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMULATIONPARAMETRES_H
#define _VSIMULATIONPARAMETRES_H

#include "VSimulationClass.h"


class VSimulationParametres {
public:
    typedef std::shared_ptr<VSimulationParametres> ptr;
    typedef std::shared_ptr<const VSimulationParametres> const_ptr;

    static const double DEFAULT_TEMPERATURE;
    static const double KELVINS_IN_0_CELSIUS;

    double getInjectionDiameter() const noexcept;
    void setInjectionDiameter(double diameter) noexcept;

    double getVacuumDiameter() const noexcept;
    void setVacuumDiameter(double diameter) noexcept;

    double getViscosity() const noexcept;

    double getDefaultViscosity() const noexcept;
    void setDefaultViscosity(double viscosity) noexcept;

    double getTemperature() const noexcept;
    void setTemperature(double temperature) noexcept;

    double getTempcoef() const noexcept;
    void setTempcoef(double tempcoef) noexcept;

    double getInjectionPressure() const noexcept;
    void setInjectionPressure(double pressure) noexcept;

    double getVacuumPressure() const noexcept;
    void setVacuumPressure(double pressure) noexcept;

    double getQ() const noexcept;
    void setQ(double q) noexcept;

    double getR() const noexcept;
    void setR(double r) noexcept;

    double getS() const noexcept;
    void setS(double s) noexcept;

    double getAveragePermeability() const noexcept;
    void setAveragePermeability(double averagePermeability) noexcept;

    double getAverageCellDistance() const noexcept;
    void setAverageCellDistance(double averageCellDistance) noexcept;

    unsigned int getNumberOfNodes() const noexcept;
    void setNumberOfNodes(unsigned int numberOfNodes) noexcept;

private:
    double m_injectionDiameter = 0;
    double m_vacuumDiameter = 0;

    double m_viscosity = 0;
    double m_temperature = 25;
    double m_tempcoef = 1;
    double m_defaultViscosity = 0;

    double m_injectionPressure = 0;
    double m_vacuumPressure = 0;

    double m_q = 1;
    double m_r = 1;
    double m_s = 2;

    double m_averagePermeability = 1;
    double m_averageCellDistance = 1;

    unsigned int m_numberOfNodes = 1;

    double calculateViscosity() const noexcept;
};

#endif //_VSIMULATIONPARAMETRES_H
