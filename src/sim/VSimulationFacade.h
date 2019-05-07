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

class VTable;
struct VInjectionVacuum;

class QWidget;

class VSimulationFacade : public QObject
{
    Q_OBJECT
public: 
    
    VSimulationFacade(QWidget *parent);
    ~VSimulationFacade();
    VSimulationFacade(const VSimulationFacade& ) = delete;
    VSimulationFacade& operator= (const VSimulationFacade& ) = delete;
    void startSimulation() ;
    void stopSimulation() ;
    void pauseSimulation() ;
    void resetSimulation() ;
    size_t getLayersNumber() const ;
    size_t getActiveLayersNumber() const ;
    size_t getInactiveLayersNumber() const ;
    size_t getNodesNumber() const;
    size_t getTrianglesNumber() const;
    void getModelSize(QVector3D &size) const;

    void setVisible(uint layer, bool visible) ;

    void removeLayer(uint layer) ;
    void enableLayer(uint layer, bool enable) ;

    void moveLayerUp(uint layer);
    void moveLayerDown(uint layer);
    void sortLayers();

    void setMaterial(uint layer, const VCloth& material) ;
    void setResin(const VResin& resin) ;
    void setInjectionPressure(double pressure) ;
    void setVacuumPressure(double pressure) ;
    void setTemperature(double temperature) ;

    void setQ(double q) ;
    void setR(double r) ;
    void setS(double s) ;

    void setTimeLimit(double limit);
    void setTimeLimitMode(bool on);
    void considerLifetime(bool on);

    void stopOnVacuumFull(bool on);
    void enableAdditionalGraphicsControls(bool on);

    void newModel() ;
    void loadModel(const QString &filename) ;
    void saveModel(const QString &filename) ;

    VCloth::const_ptr getMaterial(uint layer) const ;
    bool isLayerVisible(uint layer) const ;
    bool isLayerEnabled(uint layer) const ;

    void waitForInjectionPointSelection(float diameter);
    void waitForVacuumPointSelection(float diameter);

    void cancelWaitingForInjectionPointSelection();
    void cancelWaitingForVacuumPointSelection();

    void newLayerFromFile(const VCloth &material, const QString &filename,
                          const QString &layerName,
                          VLayerAbstractBuilder::VUnit units=VLayerAbstractBuilder::M);
    void newLayerFromPolygon(const VCloth &material, const QPolygonF &polygon,
                             double characteristicLength, const QString &layerName);
    void duplicateLayer(uint layer);

    void showInjectionPoint();
    void showVacuumPoint();

    void startCuttingLayer(uint layer);
    void cancelCuttingLayer();
    void cancelDrag();
    void performCut();
    uint getCuttedLayer() const;
    void performTransformation();
    uint getTranslatedLayerId() const;

    void setAllVisible();
    void setOnlyOneVisible(uint layer);

    VSimulationInfo getSimulationInfo() const;
    VSimulationParameters getParameters() const ;

    void loadSavedParameters();
    void saveParameters() const;

    void setCubeSide(float side);
    float getCubeSide() const;

    const QWidget * getGLWidget() const;

    bool isSimulationActive() const;
    bool isSimulationStopped() const;
    bool isSimulationPaused() const;

    std::shared_ptr<const VTable> getTable() const;
    void setTableSize(float width, float height);
    void setTableInjectionCoords(float x, float y);
    void setTableVacuumCoords(float x, float y);
    void setTableInjectionDiameter(float diameter) ;
    void setTableVacuumDiameter(float diameter) ;

    void useTableParameters(bool use);
    bool isUsingTableParameters() const;

    void setTable(const std::shared_ptr<VTable> &p_table);

    std::vector<std::vector<QPolygonF> > getAllActivePolygons() const;

    const QString & getLayerName(uint layer) const;
    void setLayerName(uint layer, const QString &name);

public slots:
    void updateGraphicsPositions();

private:
    void connectMainSignals() ;
    void initLayersProcessor() ;
    void updateConfiguration() ;
    void applyInjectionAndVacuumPoints();

    QWidget * m_parentWidget;

    VSimulator::ptr m_pSimulator;
    VGraphicsViewer::ptr m_pGraphicsViewer;
    VLayersProcessor::ptr m_pLayersProcessor;
    bool m_selectInjectionPoint;
    bool m_selectVacuumPoint;
    std::shared_ptr<VInjectionVacuum> m_pInjectionVacuum;
    std::shared_ptr<VTable> m_pTable;
    bool m_useTableParameters;
    uint m_cuttedLayer;

    std::shared_ptr<std::mutex> m_pNodesLock;
    std::shared_ptr<std::mutex> m_pTrianglesLock;

private slots:
    void m_on_got_point(const QVector3D &point);
    void m_on_got_nodes_selection(const VGraphicsViewer::const_uint_vect_ptr &pSelectedNodesIds);
    void enableInteraction();
    void disableInteraction();

signals:
    void layerVisibilityChanged(uint, bool);
    void layerRemoved(uint);
    void layerEnabled(uint, bool);
    void materialChanged(uint);
    void layerNameChanged(uint);
    void layerAdded();
    void layersCleared();
    void injectionPointSet();
    void startedWaitingForInjectionPoint();
    void canceledWaitingForInjectionPoint();
    void vacuumPointSet();
    void startedWaitingForVacuumPoint();
    void canceledWaitingForVacuumPoint();
    void simulationStarted();
    void simulationPaused();
    void simulationStopped();
    void gotNewInfo();

    void resinChanged();
    void injectionDiameterSet(float diameter) ;
    void vacuumDiameterSet(float diameter) ;
    void temperatureSet(double temperature) ;
    void vacuumPressureSet(double pressure) ;
    void injectionPressureSet(double pressure) ;
    void coefQSet(double q) ;
    void coefRSet(double r) ;
    void coefSSet(double s) ;
    void timeLimitSet(double);
    void timeLimitModeSwitched(bool);
    void lifetimeConsiderationSwitched(bool);
    void stopOnVacuumFullSwitched(bool);

    void modelSaved();
    void modelLoaded();

    void selectionMade();
    void cutPerformed();
    void translationPerformed();
    void gotTransformation();
    void cubeSideChanged(float side);
    void selectionEnabled(bool);

    void configUpdated();

    void filenameChanged(const QString &);

    void layersSwapped(uint, uint);

    void tableSizeSet(float, float);
    void tableInjectionCoordsSet(float, float);
    void tableVacuumCoordsSet(float, float);
    void tableInjectionDiameterSet(float);
    void tableVacuumDiameterSet(float);

    void useTableParametersSet(bool);

    void additionalControlsEnabled(bool);
};

#endif //_VSIMULATIONFACADE_H
