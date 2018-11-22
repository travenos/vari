/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERSPROCESSOR_H
#define _VLAYERSPROCESSOR_H

#include "VSimulationClass.h"


class VLayersProcessor: public VSimulationClass {
public: 
    
/**
 * @param simulator
 */
void VLayersProcessor(VSimulatorPtr simulator);
    
size_type getLayersNumber();
    
size_type getInactiveLayersNumber();
    
/**
 * @param builder
 */
void addLayer(VLayerAbstractBuilder& builder);
    
/**
 * @param layer
 */
void removeLayer(unsigned int layer);
    
/**
 * @param layer
 * @param visible
 */
void setVisibleLayer(unsigned int layer, bool visible);
    
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
    
/**
 * @param parametres
 */
void setParametres(VSimulationParametres& parametres);
    
double getMedianDistance();
    
void reset();
    
void clear();
private: 
    VSimulatorPtr m_pSimulator;
    vector<VLayerPtr> m_layers;
    vector<VLayerPtr> m_inactiveLayers;
    
/**
 * @param layer1
 * @param layer2
 */
void createConnections(unsigned int layer1, unsigned int layer2);
    
/**
 * @param layer1
 * @param layer2
 */
void removeConnections(unsigned int layer1, unsigned int layer2);
    
/**
 * @param layer
 */
void decreasePosition(unsigned int layer);
    
/**
 * @param layer
 */
void increasePosition(unsigned int layer);
    
/**
 * @param layer
 */
void putOnTop(unsigned int layer);
};

#endif //_VLAYERSPROCESSOR_H