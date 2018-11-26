/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VSimulationParametres.h"

/**
 * VSimulationParametres implementation
 */

VSimulationParametres::VSimulationParametres(double externalPressure,
                      double injectionDiameter,
                      double vacuumDiameter,
                      double viscosity){

}

/**
 * @return double
 */
double VSimulationParametres::getExternalPressure() const noexcept {
    return 0.0;
}

/**
 * @return double
 */
double VSimulationParametres::getInjectionDiameter() const noexcept {
    return 0.0;
}

/**
 * @return double
 */
double VSimulationParametres::getVacuumDiameter() const noexcept {
    return 0.0;
}

/**
 * @return double
 */
double VSimulationParametres::getViscosity() const noexcept {
    return 0.0;
}

/**
 * @param pressure
 */
void VSimulationParametres::setExternalPressure(double pressure) noexcept {

}

/**
 * @param diameter
 */
void VSimulationParametres::setInjectionDiameter(double diameter) noexcept {

}

/**
 * @param diameter
 */
void VSimulationParametres::setVacuumDiameter(double diameter) noexcept {

}

/**
 * @param viscosity
 */
void VSimulationParametres::setViscosity(double viscosity) noexcept {

}
