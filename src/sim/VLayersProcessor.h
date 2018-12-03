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
VLayersProcessor();
//TODO remove operator= and copy constructor
size_t getLayersNumber() const noexcept;
size_t getInactiveLayersNumber() const noexcept;
    
/**
 * @param builder
 */
void addLayer(VLayerAbstractBuilder *builder) noexcept(false);
    
/**
 * @param layer
 */
void removeLayer(unsigned int layer) noexcept(false);
    
/**
 * @param layer
 * @param visible
 */
void setVisibleLayer(unsigned int layer, bool visible) noexcept(false);
    
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
void setMaterial(unsigned int layer, const VCloth &material) noexcept(false);

void reset() noexcept;
void clear() noexcept;

const VSimNode::const_vector_ptr &getActiveNodes() const noexcept;
const VSimTriangle::const_vector_ptr &getActiveTriangles() const noexcept;

private:
    std::vector<VLayer::ptr> m_layers;
    std::vector<VLayer::ptr> m_inactiveLayers;
    VSimNode::const_vector_ptr m_pActiveNodes;
    VSimTriangle::const_vector_ptr m_pActiveTriangles;
    
/**
 * @param layer1
 * @param layer2
 */
void createConnections(unsigned int layer1, unsigned int layer2) noexcept(false);
    
/**
 * @param layer1
 * @param layer2
 */
void removeConnections(unsigned int layer1, unsigned int layer2) noexcept(false);
    
/**
 * @param layer
 */
void decreasePosition(unsigned int layer) noexcept(false);
    
/**
 * @param layer
 */
void increasePosition(unsigned int layer) noexcept(false);
    
/**
 * @param layer
 */
void putOnTop(unsigned int layer) noexcept(false);
};

#endif //_VLAYERSPROCESSOR_H
