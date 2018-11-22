/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMULATIONPARAMETRES_H
#define _VSIMULATIONPARAMETRES_H

#include "VSimulationClass.h"


class VSimulationParametres: public VSimulationClass {
public: 
    
double getExternalPressure();
    
double getInjectionDiameter();
    
double getVacuumDiameter();
    
double getViscosity();
    
/**
 * @param pressure
 */
void setExternalPressure(double pressure);
    
/**
 * @param diameter
 */
void setInjectionDiameter(double diameter);
    
/**
 * @param diameter
 */
void setVacuumDiameter(double diameter);
    
/**
 * @param viscosity
 */
void setViscosity(double viscosity);
private: 
    double m_externalPressure;
    double m_injectionDiameter;
    double m_vacuumDiameter;
    double m_viscosity;
    std::shared_mutex m_lock;
};

#endif //_VSIMULATIONPARAMETRES_H