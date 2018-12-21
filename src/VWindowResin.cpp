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
            QMessageBox::warning(this, ERROR_TITLE, e.what());
        }
    }
}

void VWindowResin::saveMaterial( )
{
    QString name;
    float viscosity, tempcoef;
    bool result = getInputs(name, viscosity, tempcoef);
    if (!result)
    {
        QMessageBox::warning(this, ERROR_TITLE, INVALID_PARAM_ERROR);
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
        QMessageBox::warning(this, ERROR_TITLE, e.what());
    }
}

void VWindowResin::accept()
{
    QString name;
    float viscosity, tempcoef;
    bool result = getInputs(name, viscosity, tempcoef);
    if (result)
    {
        hide();
        emit gotMaterial(name, viscosity, tempcoef);
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

bool VWindowResin::getInputs(QString &name, float &viscosity, float &tempcoef)
{
    name = ui->nameEdit->text();
    if (name.isEmpty())
        return false;
    bool ok;
    viscosity = ui->viscosityEdit->text().replace(',', '.').toFloat(&ok);
    if (!ok)
        return false;
    tempcoef = ui->tempcoefEdit->text().replace(',', '.').toFloat(&ok);
    return ok;
}
