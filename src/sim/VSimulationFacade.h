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
void setVisible(unsigned int layer, bool visible) ;
/**
 * @param layer
 */
void removeLayer(unsigned int layer) ;
void enableLayer(unsigned int layer, bool enable) ;
/**
 * @param layer
 * @param material
 */
void setMaterial(unsigned int layer, const VCloth& material) ;
/**
 * @param pressure
 */
void setInjectionPressure(double pressure) ;
/**
 * @param diameter
 */

void setVacuumPressure(double pressure) ;
void setDefaultViscosity(double defaultViscosity) ;
void setTemperature(double temperature) ;
void setTempcoef(double tempcoef) ;

void setQ(double q) ;
void setR(double r) ;
void setS(double s) ;
void newModel() ;

VCloth::const_ptr getMaterial(unsigned int layer) const ;
VSimulationParametres::const_ptr getParametres() const ;
bool isLayerVisible(unsigned int layer) const ;
bool isLayerEnabled(unsigned int layer) const ;

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
    void layerVisibilityChanged(unsigned int, bool);
    void layerRemoved(unsigned int);
    void layerEnabled(unsigned int, bool);
    void materialChanged(unsigned int);
    void layerAdded();
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

    void injectionDiameterSet(double diameter) ;
    void vacuumDiameterSet(double diameter) ;
    void defaultViscositySet(double defaultViscosity);
    void temperatureSet(double temperature) ;
    void tempcoefSet(double tempcoef) ;
    void vacuumPressureSet(double pressure) ;
    void injectionPressureSet(double pressure) ;
    void coefQSet(double q) ;
    void coefRSet(double r) ;
    void coefSSet(double s) ;
};

#endif //_VSIMULATIONFACADE_H
