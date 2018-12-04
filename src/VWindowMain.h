#ifndef _VWINDOWMAIN_H
#define _VWINDOWMAIN_H

#include <QMainWindow>
#include <memory>

namespace Ui {
class VWindowMain;
}
class VSimulationFacade;
class VWindowLayer;
class VCloth;
class VPolygon;

class VWindowMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit VWindowMain(QWidget *parent = nullptr);
    ~VWindowMain();

private:
    static const QString IMPORT_FROM_FILE_ERROR;
    static const QString IMPORT_WHEN_SIMULATING_ERROR;

    void showWindowLayer();
    void deleteWindowLayer();
    void addLayerFromFile(const VCloth& material,const QString& filename);
    void addLayerFromPolygon(const VCloth& material,const VPolygon& polygon);

    Ui::VWindowMain *ui;
    std::unique_ptr<VSimulationFacade> m_pFacade;
    VWindowLayer * m_pWindowLayer;

private slots:
    void m_on_layer_window_closed();
    void m_on_layer_creation_from_file_available(const VCloth& material, const QString& filename);
    void m_on_layer_creation_manual_available(const VCloth& material,const VPolygon& polygon);
    void on_addLayerButton_clicked();
};

#endif // _VWINDOWMAIN_H
