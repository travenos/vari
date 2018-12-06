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
    void on_addLayerButton_clicked();
    void on_layersListWidget_itemSelectionChanged();
    void on_layerEnableCheckBox_clicked(bool checked);
    void on_layerVisibleCheckBox_clicked(bool checked);
    void on_layerRemoveButton_clicked();
    void on_selectMaterialClothButton_clicked();
    void on_selectMaterialResinButton_clicked();
};

#endif // _VWINDOWMAIN_H
