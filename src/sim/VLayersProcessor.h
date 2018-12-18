/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERSPROCESSOR_H
#define _VLAYERSPROCESSOR_H

#include "VSimulationClass.h"
#include "VLayerAbstractBuilder.h"
#include "VLayer.h"
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
size_t getLayersNumber() const ;
size_t getActiveLayersNumber() const ;
size_t getInactiveLayersNumber() const ;
    
/**
 * @param builder
 */
void addLayer(VLayerAbstractBuilder *builder) ;
    
/**
 * @param layer
 */
void removeLayer(uint layer) ;
    
/**
 * @param layer
 * @param visible
 */
void setVisibleLayer(uint layer, bool visible) ;
    
/**
 * @param layer
 */
void enableLayer(uint layer, bool enable) ;
    
/**
 * @param layer
 * @param material
 */
void setMaterial(uint layer, const VCloth &material) ;

void reset() ;
void clear() ;

VCloth::const_ptr getMaterial(uint layer) const ;

const VSimNode::const_vector_ptr &getActiveNodes() const ;
const VSimTriangle::const_vector_ptr &getActiveTriangles() const ;

bool isLayerVisible(uint layer) const ;
bool isLayerEnabled(uint layer) const ;

void setInjectionPoint(const QVector3D &point, double diameter);
void setVacuumPoint(const QVector3D &point, double diameter);

void getActiveModelSize(QVector3D &size) const;
size_t getActiveNodesNumber() const;
size_t getActiveTrianglesNumber() const;

private:
    void updateActiveElementsVectors() ;

    std::vector<VLayer::ptr> m_layers;
    VSimNode::const_vector_ptr m_pActiveNodes;
    VSimTriangle::const_vector_ptr m_pActiveTriangles;
    
/**
 * @param layer1
 * @param layer2
 */
void createConnections(uint layer1, uint layer2) ;
    
/**
 * @param layer1
 * @param layer2
 */
void removeConnections(uint layer1, uint layer2) ;
    
/**
 * @param layer
 */
void decreasePositions(uint fromLayer) ;
    
/**
 * @param layer
 */
void increasePositions(uint fromLayer) ;
    
/**
 * @param layer
 */
void putOnTop(uint layer) ;
};

#endif //_VLAYERSPROCESSOR_H
