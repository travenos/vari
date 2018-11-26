/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMULATIONPARAMETRES_H
#define _VSIMULATIONPARAMETRES_H

#include <shared_mutex>
#include "VSimulationClass.h"


class VSimulationParametres {
public: 
    typedef std::shared_ptr<VSimulationParametres> ptr;
    typedef std::shared_ptr<const VSimulationParametres> const_ptr;
    
VSimulationParametres(double externalPressure,
                      double injectionDiameter,
                      double vacuumDiameter,
                      double viscosity);

double getExternalPressure() const noexcept;
    
double getInjectionDiameter() const noexcept;
    
double getVacuumDiameter() const noexcept;
    
double getViscosity() const noexcept;
    
/**
 * @param pressure
 */
void setExternalPressure(double pressure) noexcept;
    
/**
 * @param diameter
 */
void setInjectionDiameter(double diameter) noexcept;
    
/**
 * @param diameter
 */
void setVacuumDiameter(double diameter) noexcept;
    
/**
 * @param viscosity
 */
void setViscosity(double viscosity) noexcept;
private: 
//TODO: make structure with shared mutexes for each field
    double m_externalPressure;
    double m_injectionDiameter;
    double m_vacuumDiameter;
    double m_viscosity;
    mutable std::shared_mutex m_lock;
};

#endif //_VSIMULATIONPARAMETRES_H
