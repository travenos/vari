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

class VSimulationFacade {
public: 
    
VSimulationFacade();
void startSimulation() noexcept;
void stopSimulation() noexcept;
void resetSimulation() noexcept;
int getLayersNumber() const noexcept;
int getInactiveLayersNumber() const noexcept;
/**
 * @param layer
 * @param visible
 */
void setVisible(unsigned int layer, bool visible) noexcept;
/**
 * @param layer
 */
void removeLayer(unsigned int layer) noexcept;
/**
 * @param layer
 */
void disableLayer(unsigned int layer) noexcept;
/**
 * @param disabledLayer
 */
void enableLayer(unsigned int disabledLayer) noexcept;
/**
 * @param layer
 * @param material
 */
void setMaterial(unsigned int layer, const VMaterial& material) noexcept;
void draw();    
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
void newModel() noexcept;
/**
 * @param filename
 */
void loadFromFile(const QString &filename);
private: 
    VSimulator::mutex_::ptr m_pNodesLock;
    VSimulator::mutex_::ptr m_::ptrianglesLock;
    VSimulator::::ptr m_pSimulator;
    VGraphicsViewer::::ptr m_pGraphicsViewer;
    VLayersProcessor::::ptr m_pLayersProcessor;
    VSimulationParametres::::ptr m_pParametres;
};

#endif //_VSIMULATIONFACADE_H
