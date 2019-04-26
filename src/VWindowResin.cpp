/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif
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
    m_pValidator->setLocale(QLocale::C);
    ui->viscosityEdit->setValidator( m_pValidator );
    ui->viscTempcoefEdit->setValidator( m_pValidator );
    ui->lifetimeEdit->setValidator( m_pValidator );
    ui->lifetimeTempcoefEdit->setValidator( m_pValidator );

    setWindowTitle(TITLE);
    loadMaterials();
}

void VWindowResin::selectMaterial( )
{
    if (ui->materialsListWidget->selectionMode() != QAbstractItemView::NoSelection &&
            ui->materialsListWidget->currentIndex().isValid())
    {
        const QString &name = ui->materialsListWidget->currentItem()->text();
        float viscosity, viscTempcoef, lifetime, lifetimeTempcoef;
        try
        {
            m_pDatabaseResin->materialInfo(name, m_currentId, viscosity, viscTempcoef, lifetime, lifetimeTempcoef);
            ui->nameEdit->setText(name);
            ui->idLabel->setText(QString("ID: %1").arg(m_currentId));
            ui->viscosityEdit->setText(QString::number(viscosity));
            ui->viscTempcoefEdit->setText(QString::number(viscTempcoef));
            ui->lifetimeEdit->setText(QString::number(lifetime));
            ui->lifetimeTempcoefEdit->setText(QString::number(lifetimeTempcoef));
        }
        catch (DatabaseException &e)
        {
            QMessageBox::warning(this, ERROR_TITLE, e.what());
        }
    }
}

void VWindowResin::saveMaterial( )
{
    QString name;
    float viscosity, viscTempcoef, lifetime, lifetimeTempcoef;
    bool result = getInputs(name, viscosity, viscTempcoef, lifetime, lifetimeTempcoef);
    if (!result)
    {
        QMessageBox::warning(this, ERROR_TITLE, INVALID_PARAM_ERROR);
        return;
    }
    try
    {
        m_pDatabaseResin->saveMaterial(name, m_currentId, viscosity, viscTempcoef, lifetime, lifetimeTempcoef);
        loadMaterials();
        selectByName(name);
    }
    catch (DatabaseException &e)
    {
        QMessageBox::warning(this, ERROR_TITLE, e.what());
    }
}

void VWindowResin::accept()
{
    QString name;
    float viscosity, viscTempcoef, lifetime, lifetimeTempcoef;
    bool result = getInputs(name, viscosity, viscTempcoef, lifetime, lifetimeTempcoef);
    if (result)
    {
        hide();
        emit gotMaterial(name, viscosity, viscTempcoef, lifetime, lifetimeTempcoef);
        close();
    }
    else
        QMessageBox::warning(this, ERROR_TITLE, INVALID_PARAM_ERROR);
}

VWindowResin::~VWindowResin()
{
    delete m_pDatabaseResin;
    #ifdef DEBUG_MODE
        qInfo() << "VWindowResin destroyed";
    #endif
}

VDatabaseInteractor* VWindowResin::databaseInteractor()
{
    return m_pDatabaseResin;
}

bool VWindowResin::getInputs(QString &name, float &viscosity, float &viscTempcoef, float &lifetime,
                             float &lifetimeTempcoef)
{
    name = ui->nameEdit->text();
    if (name.isEmpty())
        return false;
    bool ok;
    viscosity = ui->viscosityEdit->text().replace(',', '.').toFloat(&ok);
    if (!ok)
        return false;
    viscTempcoef = ui->viscTempcoefEdit->text().replace(',', '.').toFloat(&ok);
    if (!ok)
        return false;
    lifetime = ui->lifetimeEdit->text().replace(',', '.').toFloat(&ok);
    if (!ok)
        return false;
    lifetimeTempcoef = ui->lifetimeTempcoefEdit->text().replace(',', '.').toFloat(&ok);
    return ok;
}
