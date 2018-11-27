#ifndef MATERIALSWINDOW_H
#define MATERIALSWINDOW_H

#include <QMainWindow>
#include <QDoubleValidator>
#include "VDatabaseInteractor.h"

namespace Ui {
class MaterialsWindow;
}

class MaterialsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MaterialsWindow(QWidget *parent = 0);
    ~MaterialsWindow();

public slots:
    void loadMaterials( );
    void selectMaterial( );
    void saveMaterial( );
    void newMaterial( );
    void removeMaterial( );
    void importMaterials( );
    void exportMaterials( );

private slots:
    void on_materialsListWidget_itemSelectionChanged();

    void on_refreshButton_clicked();

    void on_saveButton_clicked();

    void on_pushButton_clicked();

    void on_removeButton_clicked();

    void on_importButton_clicked();

    void on_exportButton_clicked();

private:
    void selectByName( const QString &name );
    Ui::MaterialsWindow *ui;
    QDoubleValidator *m_pValidator;
    DatabaseInteractor m_databaseInteractor;
    int m_currentId;
};

#endif // MATERIALSNWINDOW_H
