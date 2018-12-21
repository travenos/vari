/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMULATIONPARAMETRES_H
#define _VSIMULATIONPARAMETRES_H

#include <memory>

#include "VMaterials.h"


class VSimulationParametres
{
public:
    typedef std::shared_ptr<VSimulationParametres> ptr;
    typedef std::shared_ptr<const VSimulationParametres> const_ptr;

    static const double DEFAULT_TEMPERATURE;
    static const double KELVINS_IN_0_CELSIUS;
    static const double MIN_PRESSURE;

    VSimulationParametres() = default;
    VSimulationParametres(const VSimulationParametres &other);
    VSimulationParametres& operator= (const VSimulationParametres& other);

    double getInjectionDiameter() const ;
    void setInjectionDiameter(double diameter) ;

    double getVacuumDiameter() const ;
    void setVacuumDiameter(double diameter) ;

    void setResin(const VResin &resin);
    VResin getResin() const;

    double getViscosity() const ;

    double getDefaultViscosity() const ;

    double getTemperature() const ;
    void setTemperature(double temperature) ;

    double getTempcoef() const ;

    double getInjectionPressure() const ;
    void setInjectionPressure(double pressure) ;

    double getVacuumPressure() const ;
    void setVacuumPressure(double pressure) ;

    double getQ() const ;
    void setQ(double q) ;

    double getR() const ;
    void setR(double r) ;

    double getS() const ;
    void setS(double s) ;

    double getAverageCellDistance() const ;
    void setAverageCellDistance(double averageCellDistance) ;

    double getAveragePermeability() const ;
    void setAveragePermeability(double averagePermeability) ;

    int getNumberOfFullNodes() const ;
    void setNumberOfFullNodes(int numberOfNodes) ;

private:
    VResin m_resin;

    double m_viscosity{0.1};
    double m_temperature{25};

    double m_injectionDiameter{0.1};
    double m_vacuumDiameter{0.1};

    double m_injectionPressure{1e5};
    double m_vacuumPressure{100};

    double m_q{1};
    double m_r{1};
    double m_s{2};

    double m_averageCellDistance{1};
    double m_averagePermeability{1};

    int m_numberOfFullNodes{0};

    double calculateViscosity() const ;
};

#endif //_VSIMULATIONPARAMETRES_H
