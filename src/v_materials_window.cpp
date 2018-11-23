#include "v_materials_window.h"
#include "ui_v_materials_window.h"
#include "v_database_interactor.h"
#include <QMessageBox>
#include <QFileDialog>
#include <deque>

MaterialsWindow::MaterialsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MaterialsWindow),
    m_pValidator(new QDoubleValidator(this)),
    m_currentId(-1)
{
    ui->setupUi(this);
    ui->cavityHeightEdit->setValidator( m_pValidator );
    ui->permabilityEdit->setValidator( m_pValidator );
    ui->porosityEdit->setValidator( m_pValidator );

    loadMaterials();
}

void MaterialsWindow::loadMaterials( )
{
    ui->materialsListWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->materialsListWidget->clear();
    ui->materialsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    try
    {
        std::deque<QString> materials;
        m_databaseInteractor.getNames(materials);
        for (auto &materialName : materials)
            ui->materialsListWidget->addItem(materialName);
    }
    catch (DatabaseException &e)
    {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void MaterialsWindow::selectMaterial( )
{
    if (ui->materialsListWidget->selectionMode() != QAbstractItemView::NoSelection &&
            ui->materialsListWidget->currentIndex().isValid())
    {
        const QString &name = ui->materialsListWidget->currentItem()->text();
        float cavityheight, permability, porosity;
        try
        {
            m_databaseInteractor.materialInfo(name, m_currentId, cavityheight, permability, porosity);
            ui->nameEdit->setText(name);
            ui->idLabel->setText(QString("ID: %1").arg(m_currentId));
            ui->cavityHeightEdit->setText(QString::number(cavityheight));
            ui->permabilityEdit->setText(QString::number(permability));
            ui->porosityEdit->setText(QString::number(porosity));
        }
        catch (DatabaseException &e)
        {
            QMessageBox::warning(this, "Error", e.what());
        }
    }
}

void MaterialsWindow::saveMaterial( )
{
    bool convertOk = true;
    const QString &name = ui->nameEdit->text();
    convertOk &= !name.isEmpty();
    bool ok;
    float cavityheight = ui->cavityHeightEdit->text().toFloat(&ok);
    convertOk &= ok;
    float permability = ui->permabilityEdit->text().toFloat(&ok);
    convertOk &= ok;
    float porosity = ui->porosityEdit->text().toFloat(&ok);
    convertOk &= ok;
    if (!convertOk)
    {
        QMessageBox::warning(this, "Error", "Incorrect material parametres");
        return;
    }
    try
    {
        m_databaseInteractor.saveMaterial(name, m_currentId, cavityheight, permability, porosity);
        loadMaterials();
        selectByName(name);
    }
    catch (DatabaseException &e)
    {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void MaterialsWindow::newMaterial( )
{
    ui->materialsListWidget->clearSelection();
    ui->nameEdit->clear();
    ui->cavityHeightEdit->clear();
    ui->permabilityEdit->clear();
    ui->porosityEdit->clear();
    ui->idLabel->clear();
    m_currentId = -1;
}

void MaterialsWindow::removeMaterial( )
{
    if (m_currentId >= 0)
    {
        if (QMessageBox::question(this,"Remove?","Are you sure want to remove the material", QMessageBox::Yes|QMessageBox::No )==QMessageBox::Yes)
        {
            try
            {
                m_databaseInteractor.removeMaterial(m_currentId);
                loadMaterials();
                newMaterial();
            }
            catch (DatabaseException &e)
            {
                QMessageBox::warning(this, "Error", e.what());
            }
        }
    }
    else
        newMaterial();
}

void MaterialsWindow::importMaterials()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Import file", QDir::homePath(), "Table data (*.csv);;All files (*)");
    if (!fileName.isEmpty())
    {
        QString name;
        if (m_currentId >= 0)
            name = ui->materialsListWidget->currentItem()->text();
        try
        {
            m_databaseInteractor.loadFromFile(fileName);
            loadMaterials();
            selectByName(name);
        }
        catch (DatabaseException &e)
        {
            QMessageBox::warning(this, "Error", e.what());
        }
    }
}

void MaterialsWindow::exportMaterials()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export file", QDir::homePath(), "Table data (*.csv);;All files (*)");
    if (!fileName.isEmpty())
    {
        try
        {
            m_databaseInteractor.saveToFile(fileName);
        }
        catch (DatabaseException &e)
        {
            QMessageBox::warning(this, "Error", e.what());
        }
    }
}

void MaterialsWindow::selectByName(const QString &name)
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

MaterialsWindow::~MaterialsWindow()
{
    delete ui;
    delete m_pValidator;
}

void MaterialsWindow::on_materialsListWidget_itemSelectionChanged()
{
    selectMaterial();
}

void MaterialsWindow::on_refreshButton_clicked()
{
    if (m_currentId >= 0)
        selectMaterial();
    else
        newMaterial();
}

void MaterialsWindow::on_saveButton_clicked()
{
    saveMaterial();
}

void MaterialsWindow::on_pushButton_clicked()
{
    newMaterial();
}

void MaterialsWindow::on_removeButton_clicked()
{
    removeMaterial();
}

void MaterialsWindow::on_importButton_clicked()
{
    importMaterials();
}

void MaterialsWindow::on_exportButton_clicked()
{
    exportMaterials();
}
