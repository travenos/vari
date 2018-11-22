/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMULATIONFACADE_H
#define _VSIMULATIONFACADE_H

class VSimulationFacade {
public: 
    
void VSimulationFacade();
    
void startSimulation();
    
void stopSimulation();
    
void resetSimulation();
    
int getLayersNumber();
    
int getInactiveLayersNumber();
    
/**
 * @param layer
 * @param visible
 */
void setVisible(unsigned int layer, bool visible);
    
/**
 * @param layer
 */
void removeLayer(unsigned int layer);
    
/**
 * @param layer
 */
void disableLayer(unsigned int layer);
    
/**
 * @param disabledLayer
 */
void enableLayer(unsigned int disabledLayer);
    
/**
 * @param layer
 * @param material
 */
void setMaterial(unsigned int layer, VMaterial& material);
    
void draw();
    
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
    
void newModel();
    
/**
 * @param filename
 */
void loadFromFile(QString filename);
private: 
    VMutexPtr m_pNodesLock;
    VMutexPtr m_pTrianglesLock;
    VSimulatorPtr m_pSimulator;
    VGraphicsViewerPtr m_pGraphicsViewer;
    VLayersProcessorPtr m_pLayersProcessor;
    VSimParamPtr m_pParametres;
};

#endif //_VSIMULATIONFACADE_H