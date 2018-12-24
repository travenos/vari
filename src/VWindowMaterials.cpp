#ifdef DEBUG_MODE
#include <QDebug>
#endif
#include "VWindowMaterials.h"
#include "ui_VWindowMaterials.h"
#include "VDatabaseInteractor.h"
#include <QMessageBox>
#include <QFileDialog>
#include <deque>

const QString VWindowMaterials::ERROR_TITLE("Ошибка");
const QString VWindowMaterials::REMOVE_TITLE("Удалить?");
const QString VWindowMaterials::IMPORT_TITLE("Импорт базы данных");
const QString VWindowMaterials::EXPORT_TITLE("Экспорт базы данных");
const QString VWindowMaterials::INVALID_PARAM_ERROR("Введены некорректные параметры");
const QString VWindowMaterials::ASK_FOR_REMOVE("Вы уверены, что хотите удалить материал?");
const QString VWindowMaterials::FILE_DIALOG_FORMATS("Табличные данные (*.csv);;Все файлы (*)");


VWindowMaterials::VWindowMaterials(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VWindowMaterials),
    m_currentId(-1)
{
    ui->setupUi(this);
}

void VWindowMaterials::loadMaterials( )
{
    ui->materialsListWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->materialsListWidget->clear();
    ui->materialsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    try
    {
        std::deque<QString> materials;
        databaseInteractor()->getNames(materials);
        for (auto &materialName : materials)
            ui->materialsListWidget->addItem(materialName);
    }
    catch (DatabaseException &e)
    {
        QMessageBox::warning(this, ERROR_TITLE, e.what());
    }
}



void VWindowMaterials::removeMaterial( )
{
    if (m_currentId >= 0)
    {
        if (QMessageBox::question(this, REMOVE_TITLE, ASK_FOR_REMOVE,
                                  QMessageBox::Yes|QMessageBox::No )==QMessageBox::Yes)
        {
            try
            {
                databaseInteractor()->removeMaterial(m_currentId);
                loadMaterials();
                newMaterial();
            }
            catch (DatabaseException &e)
            {
                QMessageBox::warning(this, ERROR_TITLE, e.what());
            }
        }
    }
    else
        newMaterial();
}

void VWindowMaterials::importMaterials()
{
    QString fileName = QFileDialog::getOpenFileName(this, IMPORT_TITLE,
                                                    QDir::homePath(),
                                                    FILE_DIALOG_FORMATS);
    if (!fileName.isEmpty())
    {
        QString name;
        if (m_currentId >= 0)
            name = ui->materialsListWidget->currentItem()->text();
        try
        {
            databaseInteractor()->loadFromFile(fileName);
            loadMaterials();
            selectByName(name);
        }
        catch (DatabaseException &e)
        {
            QMessageBox::warning(this, ERROR_TITLE, e.what());
        }
    }
}

void VWindowMaterials::exportMaterials()
{
    QString fileName = QFileDialog::getSaveFileName(this, EXPORT_TITLE,
                                                    QDir::homePath(),
                                                    FILE_DIALOG_FORMATS);
    if (!fileName.isEmpty())
    {
        try
        {
            databaseInteractor()->saveToFile(fileName);
        }
        catch (DatabaseException &e)
        {
            QMessageBox::warning(this, ERROR_TITLE, e.what());
        }
    }
}

void VWindowMaterials::selectByName(const QString &name)
{
    if (!name.isEmpty())
    {
        QList <QListWidgetItem*> previousItem = ui->materialsListWidget->findItems(name, Qt::MatchExactly);
        if (previousItem.length() > 0)
        {
            QListWidgetItem* item = previousItem[0];
            ui->materialsListWidget->setCurrentItem(item);
        }
    }
}

void VWindowMaterials::newMaterial( )
{
    ui->materialsListWidget->clearSelection();
    ui->nameEdit->clear();
    ui->cavityHeightEdit->clear();
    ui->permeabilityEdit->clear();
    ui->porosityEdit->clear();
    ui->viscosityEdit->clear();
    ui->tempcoefEdit->clear();
    ui->idLabel->clear();
    m_currentId = -1;
}

void VWindowMaterials::cancelSelection()
{
    hide();
    emit selectionCanceled();
    close();
}

VWindowMaterials::~VWindowMaterials()
{
    delete ui;
    #ifdef DEBUG_MODE
        qInfo() << "VWindowMaterials destroyed";
    #endif
}

void VWindowMaterials::closeEvent(QCloseEvent *)
{
    emit windowClosed();
}

void VWindowMaterials::on_materialsListWidget_itemSelectionChanged()
{
    selectMaterial();
}

void VWindowMaterials::on_refreshButton_clicked()
{
    if (m_currentId >= 0)
        selectMaterial();
    else
        newMaterial();
}

void VWindowMaterials::on_saveButton_clicked()
{
    saveMaterial();
}

void VWindowMaterials::on_newMaterialButton_clicked()
{
    newMaterial();
}

void VWindowMaterials::on_removeButton_clicked()
{
    removeMaterial();
}

void VWindowMaterials::on_importButton_clicked()
{
    importMaterials();
}

void VWindowMaterials::on_exportButton_clicked()
{
    exportMaterials();
}

void VWindowMaterials::on_buttonBox_rejected()
{
    cancelSelection();
}

void VWindowMaterials::on_buttonBox_accepted()
{
    accept();
}

void VWindowMaterials::on_materialsListWidget_doubleClicked(const QModelIndex &)
{
    accept();
}
