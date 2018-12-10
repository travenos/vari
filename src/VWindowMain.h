#ifndef _VWINDOWMAIN_H
#define _VWINDOWMAIN_H

#include <QMainWindow>
#include <memory>

namespace Ui {
class VWindowMain;
}
class VSimulationFacade;
class VWindowLayer;
class VWindowCloth;
class VWindowResin;
struct VCloth;
struct VPolygon;

class VWindowMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit VWindowMain(QWidget *parent = nullptr);
    ~VWindowMain();

private:
    static const QString IMPORT_FROM_FILE_ERROR;
    static const QString IMPORT_WHEN_SIMULATING_ERROR;
    static const QString INVALID_PARAM_ERROR;
    static const QString CLOTH_INFO_TEXT;
    static const QString RESIN_INFO_TEXT;
    static const QString ASK_FOR_REMOVE;

    void showWindowLayer();
    void deleteWindowLayer();
    void deleteWindowCloth();
    void deleteWindowResin();
    void addLayerFromFile(const VCloth& material,const QString& filename);
    void addLayerFromPolygon(const VCloth& material,const VPolygon& polygon);
    void selectLayer();
    void enableLayer(bool enable);
    void setVisibleLayer(bool visible);
    void removeLayer();
    void showWindowCloth();
    void showWindowResin();
    void setCloth(const QString & name, float cavityheight, float permeability, float porosity);
    void setResin(const QString & name , float viscosity, float tempcoef);

    void removeLayerFromList(int layer);
    void updateLayerMaterialInfo(int layer);
    void markLayerAsEnabled(int layer, bool enable);

    void injectionPointSelectionResult();
    void vacuumPointSelectionResult();

    void startInjectionPointSelection();
    void cancelInjectionPointSelection();
    void startVacuumPointSelection();
    void cancelVacuumPointSelection();
    void startSimulation();
    void stopSimulation();
    void resetSimulationState();

    Ui::VWindowMain *ui;
    std::unique_ptr<VSimulationFacade> m_pFacade;
    VWindowLayer * m_pWindowLayer;
    VWindowCloth * m_pWindowCloth;
    VWindowResin * m_pWindowResin;

private slots:
    void m_on_layer_window_closed();
    void m_on_cloth_window_closed();
    void m_on_resin_window_closed();
    void m_on_got_cloth(const QString & name, float cavityheight, float permeability, float porosity);
    void m_on_got_resin(const QString & name , float viscosity, float tempcoef);
    void m_on_layer_creation_from_file_available(const VCloth& material, const QString& filename);
    void m_on_layer_creation_manual_available(const VCloth& material,const VPolygon& polygon);
    void m_on_layer_removed(unsigned int layer);
    void m_on_material_changed(unsigned int layer);
    void m_on_layer_enabled(unsigned int layer, bool enable);
    void m_on_injection_point_set();
    void m_on_vacuum_point_set();

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
};

#endif // _VWINDOWMAIN_H
