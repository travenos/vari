/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERSPROCESSOR_H
#define _VLAYERSPROCESSOR_H

#include "VSimulationClass.h"
#include "VLayerAbstractBuilder.h"
#include "VLayer.h"
#include "VSimulationParametres.h"
#include "VSimulator.h"


class VLayersProcessor: public VSimulationClass {
public: 
    typedef std::shared_ptr<VLayersProcessor> ptr;
    typedef std::shared_ptr<const VLayersProcessor> const_ptr;
/**
 * @param simulator
 */
VLayersProcessor(VSimulator::ptr simulator);
size_t getLayersNumber() const noexcept;
size_t getInactiveLayersNumber() const noexcept;
    
/**
 * @param builder
 */
void addLayer(VLayerAbstractBuilder& builder) noexcept;
    
/**
 * @param layer
 */
void removeLayer(unsigned int layer) noexcept;
    
/**
 * @param layer
 * @param visible
 */
void setVisibleLayer(unsigned int layer, bool visible) noexcept;
    
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
void setMaterial(unsigned int layer, VCloth& material) noexcept;
    
/**
 * @param parametres
 */
void setParametres(VSimulationParametres& parametres) noexcept;
double getMedianDistance() const noexcept;
void reset() noexcept;
void clear() noexcept;

private: 
    VSimulator::ptr m_pSimulator;
    std::vector<VLayer::ptr> m_layers;
    std::vector<VLayer::ptr> m_inactiveLayers;
    
/**
 * @param layer1
 * @param layer2
 */
void createConnections(unsigned int layer1, unsigned int layer2) noexcept;
    
/**
 * @param layer1
 * @param layer2
 */
void removeConnections(unsigned int layer1, unsigned int layer2) noexcept;
    
/**
 * @param layer
 */
void decreasePosition(unsigned int layer) noexcept;
    
/**
 * @param layer
 */
void increasePosition(unsigned int layer) noexcept;
    
/**
 * @param layer
 */
void putOnTop(unsigned int layer) noexcept;
};

#endif //_VLAYERSPROCESSOR_H
