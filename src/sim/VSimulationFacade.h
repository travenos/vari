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
/**
 * @param layer
 * @param visible
 */
void setVisible(uint layer, bool visible) ;
/**
 * @param layer
 */
void removeLayer(uint layer) ;
void enableLayer(uint layer, bool enable) ;
/**
 * @param layer
 * @param material
 */
void setMaterial(uint layer, const VCloth& material) ;
void setResin(const VResin& resin) ;
/**
 * @param pressure
 */
void setInjectionPressure(double pressure) ;
/**
 * @param diameter
 */

void setVacuumPressure(double pressure) ;
void setTemperature(double temperature) ;

void setQ(double q) ;
void setR(double r) ;
void setS(double s) ;

void newModel() ;
void loadModel(const QString &filename) ;
void saveModel(const QString &filename) ;

VCloth::const_ptr getMaterial(uint layer) const ;
VSimulationParametres::const_ptr getParametres() const ;
bool isLayerVisible(uint layer) const ;
bool isLayerEnabled(uint layer) const ;

void waitForInjectionPointSelection(double diameter);
void waitForVacuumPointSelection(double diameter);

void cancelWaitingForInjectionPointSelection();
void cancelWaitingForVacuumPointSelection();
/**
 * @param filename
 */
void newLayerFromFile(const VCloth &material, const QString &filename,
                      VLayerAbstractBuilder::VUnit units=VLayerAbstractBuilder::M);

void showInjectionPoint();
void showVacuumPoint();

VSimulationInfo getSimulationInfo() const;

void loadSavedParametres();
void saveParametres() const;

private:
    void connectSignals() ;
    void updateConfiguration() ;

    VSimulator::ptr m_pSimulator;
    VGraphicsViewer::ptr m_pGraphicsViewer;
    VLayersProcessor::ptr m_pLayersProcessor;
    bool m_selectInjectionPoint;
    bool m_selectVacuumPoint;
    double m_injectionDiameter;
    double m_vacuumDiameter;

private slots:
    void m_on_got_point(const QVector3D &point);

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

    void modelSaved();
    void modelLoaded();
};

#endif //_VSIMULATIONFACADE_H
