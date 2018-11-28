/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMULATIONPARAMETRES_H
#define _VSIMULATIONPARAMETRES_H

#include "VSimulationClass.h"


struct VSimulationParametres {
    typedef std::shared_ptr<VSimulationParametres> ptr;
    typedef std::shared_ptr<const VSimulationParametres> const_ptr;

    double injectionDiameter = 0;
    double vacuumDiameter = 0;
    double viscosity = 0;

    double injectionPressure = 0;
    double vacuumPressure = 0;

    double q = 1;
    double r = 1;
    double s = 2;

    double averagePermeability = 1;
    double averageCellDistance = 1;

    int numberOfNodes = 1;
/*
VSimulationParametres(double injectionPressure,
                      double injectionDiameter,
                      double vacuumDiameter,
                      double viscosity);

double getInjectionPressure() const noexcept;
    
double getInjectionDiameter() const noexcept;
    
double getVacuumDiameter() const noexcept;
    
double getViscosity() const noexcept;
    
void setInjectionPressure(double pressure) noexcept;
    
void setInjectionDiameter(double diameter) noexcept;
    
void setVacuumDiameter(double diameter) noexcept;
    
void setViscosity(double viscosity) noexcept;
private: 
//TODO: make structure with shared mutexes for each field
    double m_injectionPressure;
    double m_injectionDiameter;
    double m_vacuumDiameter;
    double m_viscosity;
    mutable std::shared_mutex m_lock;
*/
};

#endif //_VSIMULATIONPARAMETRES_H
