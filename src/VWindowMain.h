#ifndef _VWINDOWMAIN_H
#define _VWINDOWMAIN_H

#include <QMainWindow>
#include <memory>
#include "sim/layer_builders/VLayerAbstractBuilder.h"

namespace Ui {
class VWindowMain;
}
class QLineEdit;
class QDoubleValidator;
class VSimulationFacade;
class VWindowLayer;
class VWindowCloth;
class VWindowResin;
struct VCloth;
struct VPolygon;
struct VSimulationInfo;

class VWindowMain : public QMainWindow
{
    Q_OBJECT

public:
    static const QString ERROR_TITLE;
    static const QString REMOVE_TITLE;
    static const QString CUT_TITLE;
    static const QString TRANSFORM_TITLE;
    static const QString IMPORT_FROM_FILE_ERROR;
    static const QString EXPORT_TO_FILE_ERROR;
    static const QString IMPORT_WHEN_SIMULATING_ERROR;
    static const QString INVALID_PARAM_ERROR;
    static const QString CLOTH_INFO_TEXT;
    static const QString RESIN_INFO_TEXT;
    static const QString ASK_FOR_REMOVE;
    static const QString ASK_FOR_CUT;
    static const QString ASK_FOR_TRANSFORM;
    static const QString MODEL_INFO_TEXT;

    static const QString OPEN_FILE_DIALOG_TITLE;
    static const QString SAVE_FILE_DIALOG_TITLE;
    static const QString FILE_DIALOG_FORMATS;

    static const QColor ACTIVE_COLOR;
    static const QColor INACTIVE_COLOR;
    static const QColor INVISIBLE_COLOR;

    explicit VWindowMain(QWidget *parent = nullptr);
    ~VWindowMain();

private:
    void connectSimulationSignals();
    void setupValidators();

    void showWindowLayer();
    void deleteWindowLayer();
    void deleteWindowCloth();
    void deleteWindowResin();
    void addLayerFromFile(const VCloth& material,const QString& filename,
                          VLayerAbstractBuilder::VUnit units);
    void addLayerFromPolygon(const VCloth& material,const VPolygon& polygon,
                             VLayerAbstractBuilder::VUnit units);
    void selectLayer();
    void enableLayer(bool enable);
    void setVisibleLayer(bool visible);
    void removeLayer();
    void showWindowCloth();
    void showWindowResin();
    void setCloth(const QString & name, float cavityheight, float permeability, float porosity);
    void setResin(const QString & name , float viscosity, float tempcoef);
    void selectColor();

    void removeLayerFromList(int layer);
    void updateLayerMaterialInfo(int layer);
    void showColor(const QColor &color);
    void markLayerAsEnabled(int layer, bool enable);
    void markLayerAsVisible(int layer, bool visible);
    void reloadLayersList();

    void updateResinInfo();

    void injectionPointSelectionResult();
    void vacuumPointSelectionResult();

    void startInjectionPointSelection();
    void cancelInjectionPointSelection();
    void startVacuumPointSelection();
    void cancelVacuumPointSelection();
    void startSimulation();
    void stopSimulation();
    void pauseSimulation();
    void resetSimulationState();

    void saveTemperature();
    void saveInjectionPressure();
    void saveVacuumPressure();
    void showTemperature();
    void showInjectionPressure();
    void showInjectionDiameter();
    void showInjectionPoint();
    void showVacuumPressure();
    void showVacuumDiameter();
    void showVacuumPoint();
    void showModelInfo();
    void showNewLayer();
    void simulationStartResult();
    void simulationPauseResult();
    void simulationStopResult();

    void newModel();
    void loadModel();
    void saveModel();

    void saveTimeLimit();
    void showTimeLimit();
    void enableTimeLimitMode(bool checked);

    void activateSimControls(bool enabled);
    void resetAllInputs();

    void startCuttingLayer();
    void cancelCuttingLayer();
    void askForCut();
    void askForTransform();

    bool readNumber(const QLineEdit * lineEdit, double &output) const;

    Ui::VWindowMain *ui;
    std::unique_ptr<VSimulationFacade> m_pFacade;
    VWindowLayer * m_pWindowLayer;
    VWindowCloth * m_pWindowCloth;
    VWindowResin * m_pWindowResin;

    QDoubleValidator * const m_pTemperatureValidator;
    QDoubleValidator * const m_pPressureValidator;
    QDoubleValidator * const m_pDiameterValidator;

private slots:
    void m_on_layer_window_closed();
    void m_on_cloth_window_closed();
    void m_on_resin_window_closed();
    void m_on_layers_cleared();
    void m_on_got_cloth(const QString & name, float cavityheight, float permeability, float porosity);
    void m_on_got_resin(const QString & name , float viscosity, float tempcoef);
    void m_on_layer_creation_from_file_available(const VCloth& material, const QString& filename,
                                                 VLayerAbstractBuilder::VUnit units);
    void m_on_layer_creation_manual_available(const VCloth& material, const VPolygon& polygon,
                                              VLayerAbstractBuilder::VUnit units);
    void m_on_layer_removed(uint layer);
    void m_on_material_changed(uint layer);
    void m_on_layer_enabled(uint layer, bool enable);
    void m_on_layer_visibility_changed(uint layer, bool visible);
    void m_on_layer_added();
    void m_on_injection_point_set();
    void m_on_vacuum_point_set();
    void m_on_simutation_started();
    void m_on_simutation_paused();
    void m_on_simutation_stopped();
    void m_on_resin_changed();
    void m_on_injection_diameter_set(double);
    void m_on_vacuum_diameter_set(double);
    void m_on_temperature_set(double);
    void m_on_vacuum_pressure_set(double);
    void m_on_injection_pressure_set(double);
    void m_on_time_limit_set(double);
    void m_on_time_limit_mode_switched(bool on);
    void m_on_canceled_waiting_for_injection_point();
    void m_on_canceled_waiing_for_vacuum_point();
    void m_on_model_loaded();
    void m_on_selection_made();
    void m_on_got_transformation();
    void m_on_model_config_updated();
    void m_on_selection_enabled(bool checked);

    void on_addLayerButton_clicked();
    void on_layersListWidget_itemSelectionChanged();
    void on_layerEnableCheckBox_clicked(bool checked);
    void on_layerVisibleCheckBox_clicked(bool checked);
    void on_layerRemoveButton_clicked();
    void on_selectMaterialClothButton_clicked();
    void on_selectMaterialResinButton_clicked();
    void on_injectionPlaceButton_clicked(bool checked);
    void on_vacuumPlaceButton_clicked(bool checked);
    void on_actionStart_triggered();
    void on_actionStop_triggered();
    void on_actionReset_triggered();
    void on_resetTemperatureButton_clicked();
    void on_saveTemperatureButton_clicked();
    void on_resetInjectionPressureButton_clicked();
    void on_saveInjectionPressureButton_clicked();
    void on_resetInjectionDiameterButton_clicked();
    void on_showInjectionPlace_clicked();
    void on_resetVacuumPressureButton_clicked();
    void on_saveVacuumPressureButton_clicked();
    void on_resetVacuumDiameterButton_clicked();
    void on_showVacuumPlaceButton_clicked();
    void on_actionPause_triggered();
    void on_temperatureEdit_textEdited(const QString &);
    void on_injectionPressureEdit_textEdited(const QString &);
    void on_injectionDiameterEdit_textEdited(const QString &);
    void on_vacuumPressureEdit_textEdited(const QString &);
    void on_vacuumDiameterEdit_textEdited(const QString &);
    void on_layerColorButton_clicked();
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_saveTimeLimitButton_clicked();
    void on_resetTimeLimitButton_clicked();
    void on_timeLimitCheckBox_clicked(bool checked);
    void on_timeEdit_timeChanged(const QTime &);
    void on_layerCutButton_clicked(bool checked);
};

#endif // _VWINDOWMAIN_H
