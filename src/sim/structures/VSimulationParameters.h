/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMULATIONPARAMETERS_H
#define _VSIMULATIONPARAMETERS_H

#include <memory>

#include "VMaterials.h"


class VSimulationParameters
{
public:
    typedef std::shared_ptr<VSimulationParameters> ptr;
    typedef std::shared_ptr<const VSimulationParameters> const_ptr;

    static const double DEFAULT_TEMPERATURE;
    static const double KELVINS_IN_0_CELSIUS;
    static const double UNIVERSAL_GAS_CONSTANT;
    static const double MIN_PRESSURE;

    VSimulationParameters() = default;
    VSimulationParameters(const VSimulationParameters &other) = default;
    VSimulationParameters& operator= (const VSimulationParameters& other) = default;

    inline float getInjectionDiameter() const {return m_injectionDiameter;}
    inline float getVacuumDiameter() const {return m_vacuumDiameter;}
    inline VResin getResin() const {return m_resin;}
    inline double getViscosity() const {return m_viscosity;}
    inline double getLifetime() const {return m_lifetime;}
    inline double getDefaultViscosity() const {return m_resin.defaultViscosity;}
    inline double getDefaultLifetime() const {return m_resin.defaultLifetime;}
    inline double getTemperature() const {return m_temperature;}
    inline double getViscTempcoef() const {return m_resin.viscTempcoef;}
    inline double getLifetimeTempcoef() const {return m_resin.lifetimeTempcoef;}
    inline double getInjectionPressure() const {return m_injectionPressure;}
    inline double getVacuumPressure() const {return m_vacuumPressure;}
    inline double getQ() const {return m_q;}
    inline double getR() const {return m_r;}
    inline double getS() const {return m_s;}
    inline double getAverageCellDistance() const {return m_averageCellDistance;}
    inline double getHalfAverageCellDistance() const {return m_halfAverageCellDistance;}
    inline double getAveragePermeability() const {return m_averagePermeability;}
    inline double getTimeLimit() const {return m_timeLimit;}
    inline int getNumberOfFullNodes() const {return m_numberOfFullNodes;}


    void setInjectionDiameter(float diameter) ;
    void setVacuumDiameter(float diameter) ;
    void setResin(const VResin &resin);
    void setTemperature(double temperature) ;
    void setInjectionPressure(double pressure) ;
    void setVacuumPressure(double pressure) ;
    void setQ(double q) ;
    void setR(double r) ;
    void setS(double s) ;
    void setAverageCellDistance(double averageCellDistance) ;
    void setAveragePermeability(double averagePermeability) ;
    void setTimeLimit(double limit) ;
    void setNumberOfFullNodes(int numberOfNodes) ;

private:
    VResin m_resin;

    double m_viscosity{0.1};
    double m_lifetime{10800};
    double m_temperature{25};

    float m_injectionDiameter{0.02f};
    float m_vacuumDiameter{0.02f};

    double m_injectionPressure{1e5};
    double m_vacuumPressure{100};

    double m_q{1};
    double m_r{1};
    double m_s{2};

    double m_averageCellDistance{1};
    double m_halfAverageCellDistance{0.5};
    double m_averagePermeability{1};

    double m_timeLimit{7200};

    int m_numberOfFullNodes{0};

    double calculateViscosity() const ;
    double calculateLifetime() const;
};

#endif //_VSIMULATIONPARAMETERS_H
