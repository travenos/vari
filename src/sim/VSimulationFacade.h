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
int getActiveLayersNumber() const noexcept;
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
void enableLayer(unsigned int layer, bool enable) noexcept(false);
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

void setDefaultViscosity(double defaultViscosity) noexcept;
void setTemperature(double temperature) noexcept;
void setTempcoef(double tempcoef) noexcept;

void setQ(double q) noexcept;
void setR(double r) noexcept;
void setS(double s) noexcept;
void newModel() noexcept;

VCloth::const_ptr getMaterial(unsigned int layer) const noexcept(false);
VSimulationParametres::const_ptr getParametres() const noexcept;
bool isLayerVisible(unsigned int layer) const noexcept(false);
bool isLayerEnabled(unsigned int layer) const noexcept(false);
/**
 * @param filename
 */
void newLayerFromFile(const VCloth &material, const QString &filename) noexcept(false) ;

private:
    void updateConfiguration() noexcept(false);

    VSimulator::ptr m_pSimulator;
    VGraphicsViewer::ptr m_pGraphicsViewer;
    VLayersProcessor::ptr m_pLayersProcessor;
};

#endif //_VSIMULATIONFACADE_H
