/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VWINDOWLAYER_H
#define _VWINDOWLAYER_H

#include <QMainWindow>

#include "sim/layer_builders/VLayerAbstractBuilder.h"

namespace Ui {
class VWindowLayer;
}
class VWindowCloth;
class VWindowPolygon;
class VSimulationFacade;

class VWindowLayer : public QMainWindow
{
    Q_OBJECT

public:
    static const QString NO_MATERIAL_TEXT;
    static const QString NO_GEOMETRY_TEXT;
    static const QString COLOR_DIALOG_TITLE;
    static const QString FILE_DIALOG_TITLE;
    static const QString FILE_DIALOG_FORMATS;
    static const QString GEOMETRY_FROM_FILE_TEXT;
    static const QString GEOMETRY_MANUAL_TEXT;
    static const QString MATERIAL_NAME_TEXT;
    static const QString MATERIAL_INFO_TEXT;

    static const QColor DEFAULT_COLOR;

    VWindowLayer(QWidget *parent = nullptr,
                 std::shared_ptr<const VSimulationFacade> p_facade = nullptr);
    virtual ~VWindowLayer();
    void saveParameters() const;
    void loadSavedParameters();

public slots:

private slots:
    void m_on_got_material(const QString &name, float cavityheight, float permeability, float porosity);

    void m_on_got_polygon(const QPolygonF &polygon, double characteristicLength);

    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

    void on_createFromFileButton_clicked();

    void on_chooseMaterialButton_clicked();

    void on_colorButton_clicked();

    void on_createManualButton_clicked();

private:
    void reset();
    void reject();
    void accept();
    void updateButtonsStates();
    void openFile();
    void showWindowCloth();
    void showWindowPolygon();
    void selectColor();
    void showColor();
    virtual void closeEvent(QCloseEvent *) override;

    Ui::VWindowLayer *ui;
    std::unique_ptr<VWindowCloth> m_pWindowCloth;
    std::unique_ptr<VWindowPolygon> m_pWindowPolygon;
    bool m_selectedMaterial;
    bool m_selectedFile;
    bool m_createdGeometry;
    QString m_filename;
    QString m_lastDir;
    VCloth m_material;
    QPolygonF m_polygon;
    double m_characteristicLength;
    std::shared_ptr<const VSimulationFacade> m_pFacade;

signals:
    void creationFromFileAvailable(const VCloth &material, const QString &fileName,
                                   VLayerAbstractBuilder::VUnit units);
    void creationManualAvailable(const VCloth &material, const QPolygonF &polygon,
                                 double characteristicLength);
    void creationCanceled();
    void windowClosed();
};

#endif // _VWINDOWLAYER_H
