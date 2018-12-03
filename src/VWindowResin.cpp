#include <QMessageBox>
#include <QDoubleValidator>
#include "VWindowResin.h"
#include "VDatabaseResin.h"
#include "ui_VWindowMaterials.h"

const QString VWindowResin::TITLE("База данных связующих");

VWindowResin::VWindowResin(QWidget *parent):
    VWindowMaterials(parent),
    m_pDatabaseResin(new VDatabaseResin),
    m_pValidator(new QDoubleValidator())
{
    ui->permeabilityEdit->hide();
    ui->permeabilityLabel->hide();
    ui->porosityEdit->hide();
    ui->porosityLabel->hide();
    ui->cavityHeightEdit->hide();
    ui->cavityHeightLabel->hide();

    m_pValidator->setBottom(0);
    ui->viscosityEdit->setValidator( m_pValidator );
    ui->tempcoefEdit->setValidator( m_pValidator );

    setWindowTitle(TITLE);
    loadMaterials();
}

void VWindowResin::selectMaterial( )
{
    if (ui->materialsListWidget->selectionMode() != QAbstractItemView::NoSelection &&
            ui->materialsListWidget->currentIndex().isValid())
    {
        const QString &name = ui->materialsListWidget->currentItem()->text();
        float viscosity;
        float tempcoef;
        try
        {
            m_pDatabaseResin->materialInfo(name, m_currentId, viscosity, tempcoef);
            ui->nameEdit->setText(name);
            ui->idLabel->setText(QString("ID: %1").arg(m_currentId));
            ui->viscosityEdit->setText(QString::number(viscosity));
            ui->tempcoefEdit->setText(QString::number(tempcoef));
        }
        catch (DatabaseException &e)
        {
            QMessageBox::warning(this, "Error", e.what());
        }
    }
}

void VWindowResin::saveMaterial( )
{
    bool convertOk = true;
    const QString &name = ui->nameEdit->text();
    convertOk &= !name.isEmpty();
    bool ok;
    float viscosity = ui->viscosityEdit->text().toFloat(&ok);
    convertOk &= ok;
    float tempcoef = ui->tempcoefEdit->text().toFloat(&ok);
    convertOk &= ok;
    if (!convertOk)
    {
        QMessageBox::warning(this, "Error", INVALID_PARAM_ERROR);
        return;
    }
    try
    {
        m_pDatabaseResin->saveMaterial(name, m_currentId, viscosity, tempcoef);
        loadMaterials();
        selectByName(name);
    }
    catch (DatabaseException &e)
    {
        QMessageBox::warning(this, "Error", e.what());
    }
}

VWindowResin::~VWindowResin()
{
    delete m_pDatabaseResin;
}

VDatabaseInteractor* VWindowResin::databaseInteractor()
{
    return m_pDatabaseResin;
}
