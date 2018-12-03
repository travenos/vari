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
    m_pPartValidator->setBottom(0);
    m_pPartValidator->setTop(1);
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
            QMessageBox::warning(this, "Error", e.what());
        }
    }
}

void VWindowCloth::saveMaterial( )
{
    bool convertOk = true;
    const QString &name = ui->nameEdit->text();
    convertOk &= !name.isEmpty();
    bool ok;
    float cavityheight = ui->cavityHeightEdit->text().toFloat(&ok);
    convertOk &= ok;
    float permeability = ui->permeabilityEdit->text().toFloat(&ok);
    convertOk &= ok;
    QString porosityStr = ui->porosityEdit->text();
    float porosity;
    int pos = 0;
    QValidator::State state = ui->porosityEdit->validator()->validate(porosityStr, pos);
    if (state == QValidator::Acceptable)
        porosity = porosityStr.toDouble(&ok);
    else
        convertOk = false;
    if (!convertOk)
    {
        QMessageBox::warning(this, "Error", INVALID_PARAM_ERROR);
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
        QMessageBox::warning(this, "Error", e.what());
    }
}

VWindowCloth::~VWindowCloth()
{
    delete m_pDatabaseCloth;
}

VDatabaseInteractor* VWindowCloth::databaseInteractor()
{
    return m_pDatabaseCloth;
}
