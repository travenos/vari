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
    static const QString ERROR_TITLE;
    static const QString INVALID_PARAM_ERROR;
    static const QString ASK_FOR_REMOVE;

    VWindowMaterials(QWidget *parent = nullptr);
    virtual ~VWindowMaterials();

private slots:
    void on_materialsListWidget_itemSelectionChanged();

    void on_refreshButton_clicked();

    void on_saveButton_clicked();

    void on_removeButton_clicked();

    void on_importButton_clicked();

    void on_exportButton_clicked();

    void on_buttonBox_rejected();

    void on_newMaterialButton_clicked();

    void on_buttonBox_accepted();

    void on_materialsListWidget_doubleClicked(const QModelIndex &);

protected:
    Ui::VWindowMaterials *ui;
    int m_currentId;

    virtual void selectMaterial( ) = 0;
    virtual void saveMaterial( ) = 0;
    virtual void accept() = 0;
    virtual void closeEvent(QCloseEvent *);
    void loadMaterials( );
    void newMaterial( );
    void removeMaterial( );
    void importMaterials( );
    void exportMaterials( );
    void selectByName( const QString &name );
    void cancelSelection( );
private:
    virtual VDatabaseInteractor* databaseInteractor() = 0;

signals:
    void selectionCanceled();
    void windowClosed();
};

#endif // _VWINDOWMATERIALS_H
