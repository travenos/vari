/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif
#include <QMessageBox>
#include <QDoubleValidator>
#include "VWindowCloth.h"
#include "VDatabaseCloth.h"
#include "ui_VWindowMaterials.h"

const QString VWindowCloth::TITLE("База данных тканей");

VWindowCloth::VWindowCloth(QWidget *parent):
    VWindowMaterials(parent),
    m_pDatabaseCloth(new VDatabaseCloth),
    m_pValidator(new QDoubleValidator),
    m_pPartValidator(new QDoubleValidator())
{
    ui->viscosityEdit->hide();
    ui->viscosityLabel->hide();
    ui->tempcoefEdit->hide();
    ui->tempcoefLabel->hide();

    m_pValidator->setBottom(0);
    m_pValidator->setLocale(QLocale::C);
    m_pPartValidator->setBottom(0);
    m_pPartValidator->setTop(1);
    m_pPartValidator->setLocale(QLocale::C);
    ui->cavityHeightEdit->setValidator( m_pValidator );
    ui->permeabilityEdit->setValidator( m_pValidator );
    ui->porosityEdit->setValidator( m_pPartValidator );

    setWindowTitle(TITLE);
    loadMaterials();
}

void VWindowCloth::selectMaterial( )
{
    if (ui->materialsListWidget->selectionMode() != QAbstractItemView::NoSelection &&
            ui->materialsListWidget->currentIndex().isValid())
    {
        const QString &name = ui->materialsListWidget->currentItem()->text();
        float cavityheight, permeability, porosity;
        try
        {
            m_pDatabaseCloth->materialInfo(name, m_currentId, cavityheight, permeability, porosity);
            ui->nameEdit->setText(name);
            ui->idLabel->setText(QString("ID: %1").arg(m_currentId));
            ui->cavityHeightEdit->setText(QString::number(cavityheight));
            ui->permeabilityEdit->setText(QString::number(permeability));
            ui->porosityEdit->setText(QString::number(porosity));
        }
        catch (DatabaseException &e)
        {
            QMessageBox::warning(this, ERROR_TITLE, e.what());
        }
    }
}

void VWindowCloth::saveMaterial( )
{
    QString name;
    float cavityheight, permeability, porosity;
    bool result = getInputs(name, cavityheight, permeability, porosity);
    if (!result)
    {
        QMessageBox::warning(this, ERROR_TITLE, INVALID_PARAM_ERROR);
        return;
    }
    try
    {
        m_pDatabaseCloth->saveMaterial(name, m_currentId, cavityheight, permeability, porosity);
        loadMaterials();
        selectByName(name);
    }
    catch (DatabaseException &e)
    {
        QMessageBox::warning(this, ERROR_TITLE, e.what());
    }
}

void VWindowCloth::accept()
{
    QString name;
    float cavityheight, permeability, porosity;
    bool result = getInputs(name, cavityheight, permeability, porosity);
    if (result)
    {
        hide();
        emit gotMaterial(name, cavityheight, permeability, porosity);
        close();
    }
    else
        QMessageBox::warning(this, ERROR_TITLE, INVALID_PARAM_ERROR);
}

VWindowCloth::~VWindowCloth()
{
    delete m_pDatabaseCloth;
    #ifdef DEBUG_MODE
        qInfo() << "VWindowCloth destroyed";
    #endif
}

VDatabaseInteractor* VWindowCloth::databaseInteractor()
{
    return m_pDatabaseCloth;
}

bool VWindowCloth::getInputs(QString &name, float &cavityheight, float &permeability, float &porosity)
{
    name = ui->nameEdit->text();
    if (name.isEmpty())
        return false;
    bool ok;
    cavityheight = ui->cavityHeightEdit->text().replace(',', '.').toFloat(&ok);
    if (!ok)
        return false;
    permeability = ui->permeabilityEdit->text().replace(',', '.').toFloat(&ok);
    if (!ok)
        return false;
    QString porosityStr = ui->porosityEdit->text().replace(',', '.');
    int pos = 0;
    QValidator::State state = ui->porosityEdit->validator()->validate(porosityStr, pos);
    if (state == QValidator::Acceptable)
        porosity = porosityStr.toDouble(&ok);
    else
        return false;
    return ok;
}
