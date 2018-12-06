#ifndef _VWINDOWLAYER_H
#define _VWINDOWLAYER_H

#include <QMainWindow>
#include "sim/VCloth.h"
#include "sim/VGeometryProcessor.h"

namespace Ui {
class VWindowLayer;
}
class VWindowCloth;

class VWindowLayer : public QMainWindow
{
    Q_OBJECT

public:
    VWindowLayer(QWidget *parent = nullptr);
    virtual ~VWindowLayer();

public slots:

private slots:
    void m_on_got_material(const QString &name, float cavityheight, float permeability, float porosity);

    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

    void on_createFromFileButton_clicked();

    void on_chooseMaterialButton_clicked();

private:
    static const QString NO_MATERIAL_TEXT;
    static const QString NO_GEOMETRY_TEXT;
    static const QString FILE_DIALOG_TITLE;
    static const QString FILE_DIALOG_FORMATS;
    static const QString GEOMETRY_FROM_FILE_TEXT;
    static const QString GEOMETRY_MANUAL_TEXT;
    static const QString MATERIAL_NAME_TEXT;
    static const QString MATERIAL_INFO_TEXT;
    //static const QString IMPORT_ERROR_TEXT; //TODO remove

    static const QColor DEFAULT_COLOR;

    void reset();
    void reject();
    void accept();
    void tryToEnableAcceptButton();
    void openFile();
    void showWindowCloth();
    virtual void closeEvent(QCloseEvent *);

    Ui::VWindowLayer *ui;
    std::unique_ptr<VWindowCloth> m_pWindowCloth;
    QColor m_color;
    bool m_selectedMaterial;
    bool m_selectedFile;
    bool m_createdGeometry;
    QString m_filename;
    VCloth m_material;
    VPolygon m_polygon;

signals:
    void creationFromFileAvailable(const VCloth &material, const QString &fileName);
    void creationManualAvailable(const VCloth &material, const VPolygon &polygon);
    void creationCanceled();
    void windowClosed();
};

#endif // _VWINDOWLAYER_H