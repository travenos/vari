/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMULATIONFACADE_H
#define _VSIMULATIONFACADE_H

#include <memory>
#include <mutex>
#include "VSimulator.h"
#include "VLayersProcessor.h"
#include "VGraphicsViewer.h"

class QWidget;

class VSimulationFacade {
public: 
    
VSimulationFacade(QWidget *parent);
//TODO remove operator= and copy constructor
void startSimulation() noexcept;
void stopSimulation() noexcept;
void resetSimulation() noexcept;
int getLayersNumber() const noexcept;
int getInactiveLayersNumber() const noexcept;
/**
 * @param layer
 * @param visible
 */
void setVisible(unsigned int layer, bool visible) noexcept(false);
/**
 * @param layer
 */
void removeLayer(unsigned int layer) noexcept(false);
/**
 * @param layer
 */
void disableLayer(unsigned int layer) noexcept(false);
/**
 * @param disabledLayer
 */
void enableLayer(unsigned int disabledLayer) noexcept(false);
/**
 * @param layer
 * @param material
 */
void setMaterial(unsigned int layer, const VCloth& material) noexcept(false);
void draw() noexcept;
/**
 * @param pressure
 */
void setInjectionPressure(double pressure) noexcept;
/**
 * @param diameter
 */

void setVacuumPressure(double pressure) noexcept;

void setInjectionDiameter(double diameter) noexcept;
/**
 * @param diameter
 */
void setVacuumDiameter(double diameter) noexcept;
    
/**
 * @param viscosity
 */
void setViscosity(double viscosity) noexcept;

void setQ(double q) noexcept;
void setR(double r) noexcept;
void setS(double s) noexcept;
void newModel() noexcept;
/**
 * @param filename
 */
void newLayerFromFile(const QString &filename, const VCloth &material) noexcept(false) ;

private:
    void updateConfiguration() noexcept(false);

    VSimulator::ptr m_pSimulator;
    VGraphicsViewer::ptr m_pGraphicsViewer;
    VLayersProcessor::ptr m_pLayersProcessor;
};

#endif //_VSIMULATIONFACADE_H
