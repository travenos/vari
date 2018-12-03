#ifndef _VWINDOWMATERIALS_H
#define _VWINDOWMATERIALS_H

#include <QMainWindow>
#include "VDatabaseInteractor.h"

namespace Ui {
class VWindowMaterials;
}

class VWindowMaterials : public QMainWindow
{
    Q_OBJECT

public:
    VWindowMaterials(QWidget *parent = nullptr);
    virtual ~VWindowMaterials();

public slots:
    virtual void selectMaterial( ) = 0;
    virtual void saveMaterial( ) = 0;
    void loadMaterials( );
    void newMaterial( );
    void removeMaterial( );
    void importMaterials( );
    void exportMaterials( );

private slots:
    void on_materialsListWidget_itemSelectionChanged();

    void on_refreshButton_clicked();

    void on_saveButton_clicked();

    void on_removeButton_clicked();

    void on_importButton_clicked();

    void on_exportButton_clicked();

    void on_buttonBox_rejected();

    void on_newMaterialButton_clicked();

protected:
    static const QString INVALID_PARAM_ERROR;
    static const QString ASK_FOR_REMOVE;

    Ui::VWindowMaterials *ui;
    int m_currentId;

    void selectByName( const QString &name );
private:
    virtual VDatabaseInteractor* databaseInteractor() = 0;
};

#endif // _VWINDOWMATERIALS_H
