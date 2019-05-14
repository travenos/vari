/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif
#include <QMessageBox>
#include "VWindowResin.h"
#include "VDatabaseResin.h"
#include "ui_VWindowMaterials.h"

const QString VWindowResin::TITLE("База данных связующих");

const QString VWindowResin::PARAM1_LABEL("Вязкость при 25 °C (Па·с):");
const QString VWindowResin::PARAM2_LABEL("Энергия активации вязкого течения (Дж/моль):");
const QString VWindowResin::PARAM3_LABEL("Время жизни при 25 °C (с):");
const QString VWindowResin::PARAM4_LABEL("Энергия активации процесса отверждения (Дж/моль):");

VWindowResin::VWindowResin(QWidget *parent):
    VWindowMaterials(parent),
    m_pDatabaseResin(new VDatabaseResin)
{
    assignUiPointers();
    setTitles();
    loadMaterials();
}

inline void VWindowResin::assignUiPointers()
{
    m_ui.viscosityEdit = ui->param1Edit;
    m_ui.viscTempcoefEdit = ui->param2Edit;
    m_ui.lifetimeEdit = ui->param3Edit;
    m_ui.lifetimeTempcoefEdit = ui->param4Edit;

    m_ui.viscosityLabel = ui->param1Label;
    m_ui.viscTempcoefLabel = ui->param2Label;
    m_ui.lifetimeLabel = ui->param3Label;
    m_ui.lifetimeTempcoefLabel = ui->param4Label;
}

inline void VWindowResin::setTitles()
{
    setWindowTitle(TITLE);

    ui->param1Label->setText(PARAM1_LABEL);
    ui->param2Label->setText(PARAM2_LABEL);
    ui->param3Label->setText(PARAM3_LABEL);
    ui->param4Label->setText(PARAM4_LABEL);
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
            m_ui.viscosityEdit->setText(QString::number(viscosity));
            m_ui.viscTempcoefEdit->setText(QString::number(viscTempcoef));
            m_ui.lifetimeEdit->setText(QString::number(lifetime));
            m_ui.lifetimeTempcoefEdit->setText(QString::number(lifetimeTempcoef));
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
    viscosity = m_ui.viscosityEdit->text().replace(',', '.').toFloat(&ok);
    if (!ok)
        return false;
    viscTempcoef = m_ui.viscTempcoefEdit->text().replace(',', '.').toFloat(&ok);
    if (!ok)
        return false;
    lifetime = m_ui.lifetimeEdit->text().replace(',', '.').toFloat(&ok);
    if (!ok)
        return false;
    lifetimeTempcoef = m_ui.lifetimeTempcoefEdit->text().replace(',', '.').toFloat(&ok);
    return ok;
}
