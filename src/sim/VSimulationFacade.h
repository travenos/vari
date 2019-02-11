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

    void newModel() ;
    void loadModel(const QString &filename) ;
    void saveModel(const QString &filename) ;

    VCloth::const_ptr getMaterial(uint layer) const ;
    bool isLayerVisible(uint layer) const ;
    bool isLayerEnabled(uint layer) const ;

    void waitForInjectionPointSelection(double diameter);
    void waitForVacuumPointSelection(double diameter);

    void cancelWaitingForInjectionPointSelection();
    void cancelWaitingForVacuumPointSelection();

    void newLayerFromFile(const VCloth &material, const QString &filename,
                          VLayerAbstractBuilder::VUnit units=VLayerAbstractBuilder::M);
    void newLayerFromPolygon(const VCloth &material, const QPolygonF &polygon,
                             double characteristicLength);

    void showInjectionPoint();
    void showVacuumPoint();

    void startCuttingLayer(uint layer);
    void cancelCuttingLayer();
    void cancelDrag();
    void performCut();
    uint getCuttedLayer() const;
    void performTransformation();
    uint getTranslatedLayer() const;

    void setAllVisible();
    void setOnlyOneVisible(uint layer);

    VSimulationInfo getSimulationInfo() const;
    VSimulationParameters getParameters() const ;

    void loadSavedParameters();
    void saveParameters() const;

public slots:
    void updateGraphicsPositions();

private:
    void connectMainSignals() ;
    void initLayersProcessor() ;
    void updateConfiguration() ;

    QWidget * m_parentWidget;

    VSimulator::ptr m_pSimulator;
    VGraphicsViewer::ptr m_pGraphicsViewer;
    VLayersProcessor::ptr m_pLayersProcessor;
    bool m_selectInjectionPoint;
    bool m_selectVacuumPoint;
    double m_injectionDiameter;
    double m_vacuumDiameter;
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
    void injectionDiameterSet(double diameter) ;
    void vacuumDiameterSet(double diameter) ;
    void temperatureSet(double temperature) ;
    void vacuumPressureSet(double pressure) ;
    void injectionPressureSet(double pressure) ;
    void coefQSet(double q) ;
    void coefRSet(double r) ;
    void coefSSet(double s) ;
    void timeLimitSet(double);
    void timeLimitModeSwitched(bool);

    void modelSaved();
    void modelLoaded();

    void selectionMade();
    void cutPerformed();
    void translationPerformed();
    void gotTransformation();
    void selectionEnabled(bool);

    void configUpdated();
};

#endif //_VSIMULATIONFACADE_H
