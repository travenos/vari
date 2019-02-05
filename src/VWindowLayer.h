/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VWINDOWLAYER_H
#define _VWINDOWLAYER_H

#include <QMainWindow>

#include "sim/layer_builders/VLayerManualBuilder.h"

namespace Ui {
class VWindowLayer;
}
class VWindowCloth;
class VWindowPolygon;

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

    VWindowLayer(QWidget *parent = nullptr);
    virtual ~VWindowLayer();
    void saveParametres() const;
    void loadSavedParametres();

public slots:

private slots:
    void m_on_got_material(const QString &name, float cavityheight, float permeability, float porosity);

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
    void tryToEnableAcceptButton();
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
    VPolygon m_polygon;

signals:
    void creationFromFileAvailable(const VCloth &material, const QString &fileName,
                                   VLayerAbstractBuilder::VUnit units);
    void creationManualAvailable(const VCloth &material, const VPolygon &polygon,
                                 VLayerAbstractBuilder::VUnit units);
    void creationCanceled();
    void windowClosed();
};

#endif // _VWINDOWLAYER_H
