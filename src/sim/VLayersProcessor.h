/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERSPROCESSOR_H
#define _VLAYERSPROCESSOR_H

#include <deque>

#include "VSimulationClass.h"
#include "layer_builders/VLayerAbstractBuilder.h"
#include "sim_elements/VLayer.h"
#include "VSimulator.h"

class VLayersProcessor: public VSimulationClass
{
    Q_OBJECT
public: 
    typedef std::shared_ptr<VLayersProcessor> ptr;
    typedef std::shared_ptr<const VLayersProcessor> const_ptr;
    VLayersProcessor();
    size_t getLayersNumber() const ;
    size_t getActiveLayersNumber() const ;
    size_t getInactiveLayersNumber() const ;

    void addLayer(VLayerAbstractBuilder *builder) ;

    void removeLayer(uint layer) ;

    void setVisibleLayer(uint layer, bool visible) ;

    void enableLayer(uint layer, bool enable) ;

    void setMaterial(uint layer, const VCloth &material) ;

    void moveUp(uint layer);
    void moveDown(uint layer);
    void sort();

    void reset() ;
    void clear() ;    

    void setLayers (const std::deque<VLayer::ptr> &layers) ;

    VCloth::const_ptr getMaterial(uint layer) const ;

    const VSimNode::const_vector_ptr &getActiveNodes() const ;
    const VSimTriangle::const_vector_ptr &getActiveTriangles() const ;

    VSimNode::const_map_ptr getLayerNodes(uint layer) const ;
    VSimTriangle::const_list_ptr getLayerTriangles(uint layer) const ;
    size_t getLayerNodesNumber(uint layer) const ;
    size_t getLayerTrianglesNumber(uint layer) const ;
    std::vector<VLayer::const_ptr> getLayers() const;

    bool isLayerVisible(uint layer) const ;
    bool isLayerEnabled(uint layer) const ;

    bool areLayersConnected() const;

    void setInjectionPoint(const QVector2D &point, double diameter);
    void setVacuumPoint(const QVector2D &point, double diameter);

    void getActiveModelSize(QVector3D &size) const;
    size_t getActiveNodesNumber() const;
    size_t getActiveTrianglesNumber() const;

    uint getNodeMinId(uint layer) const;
    uint getNodeMaxId(uint layer) const;
    uint getTriangleMinId(uint layer) const;
    uint getTriangleMaxId(uint layer) const;

    void cutLayer(const std::shared_ptr<const std::vector<uint> > &nodesIds, uint layer);
    void transformateLayer(const std::shared_ptr<const std::vector<std::pair<uint, QVector3D> > >
                        &nodesCoords, uint layer);

    int getLayerNumberById(uint layerId) const;
    uint getLayerId(uint layer) const;

    void createConnections() ;
    void removeConnections() ;
    void resetNodesVolumes() ;

private:
    void updateActiveElementsVectors() ;
    void updateNextIds();

    std::deque<VLayer::ptr> m_layers;
    VSimNode::const_vector_ptr m_pActiveNodes;
    VSimTriangle::const_vector_ptr m_pActiveTriangles;

    uint m_layerNextId;
    uint m_nodeNextId;
    uint m_triangleNextId;

    bool m_layersConnected;

    void putOnTop(uint layer) ;
    void sortLayers(uint fromLayer);

    void setPoint(const QVector2D &point, double diameter,
                  VSimNode::VNodeRole setRole, VSimNode::VNodeRole anotherRole);
signals:
    void layerVisibilityChanged(uint, bool);
    void layerRemoved(uint);
    void layerEnabled(uint, bool);
    void materialChanged(uint);
    void layerAdded();
    void layersCleared();
    void layersLoaded();
};

#endif //_VLAYERSPROCESSOR_H
