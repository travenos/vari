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

const QString VWindowCloth::PARAM1_LABEL("Толщина (м):");
const QString VWindowCloth::PARAM2_LABEL("<html><head/><body>"
                                         "Продольная проницаемость (м<span style=\" vertical-align:super;\">2</span>):"
                                         "</body></html>");
const QString VWindowCloth::PARAM3_LABEL("<html><head/><body>"
                                         "Поперечная проницаемость (м<span style=\" vertical-align:super;\">2</span>):"
                                         "</body></html>");
const QString VWindowCloth::PARAM4_LABEL("Пористость (от 0 до 1):");

VWindowCloth::VWindowCloth(QWidget *parent):
    VWindowMaterials(parent),
    m_pDatabaseCloth(new VDatabaseCloth),
    m_pPartValidator(new QDoubleValidator())
{
    assignUiPointers();
    setTitles();

    m_pPartValidator->setBottom(0);
    m_pPartValidator->setTop(1);
    m_pPartValidator->setLocale(QLocale::C);
    m_ui.porosityEdit->setValidator( m_pPartValidator );
    loadMaterials();
}

inline void VWindowCloth::assignUiPointers()
{
    m_ui.cavityHeightEdit = ui->param1Edit;
    m_ui.xPermeabilityEdit = ui->param2Edit;
    m_ui.yPermeabilityEdit = ui->param3Edit;
    m_ui.porosityEdit = ui->param4Edit;

    m_ui.cavityHeightLabel = ui->param1Label;
    m_ui.xPermeabilityLabel = ui->param2Label;
    m_ui.yPermeabilityLabel = ui->param3Label;
    m_ui.porosityLabel = ui->param4Label;
}

inline void VWindowCloth::setTitles()
{
    setWindowTitle(TITLE);

    ui->param1Label->setText(PARAM1_LABEL);
    ui->param2Label->setText(PARAM2_LABEL);
    ui->param3Label->setText(PARAM3_LABEL);
    ui->param4Label->setText(PARAM4_LABEL);
}

void VWindowCloth::selectMaterial( )
{
    if (ui->materialsListWidget->selectionMode() != QAbstractItemView::NoSelection &&
            ui->materialsListWidget->currentIndex().isValid())
    {
        const QString &name = ui->materialsListWidget->currentItem()->text();
        float cavityheight, xpermeability, ypermeability, porosity;
        try
        {
            m_pDatabaseCloth->materialInfo(name, m_currentId, cavityheight, xpermeability,
                                           ypermeability, porosity);
            ui->nameEdit->setText(name);
            ui->idLabel->setText(QString("ID: %1").arg(m_currentId));
            m_ui.cavityHeightEdit->setText(QString::number(cavityheight));
            m_ui.xPermeabilityEdit->setText(QString::number(xpermeability));
            m_ui.yPermeabilityEdit->setText(QString::number(ypermeability));
            m_ui.porosityEdit->setText(QString::number(porosity));
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
    float cavityheight, xpermeability, ypermeability, porosity;
    bool result = getInputs(name, cavityheight, xpermeability, ypermeability, porosity);
    if (!result)
    {
        QMessageBox::warning(this, ERROR_TITLE, INVALID_PARAM_ERROR);
        return;
    }
    try
    {
        m_pDatabaseCloth->saveMaterial(name, m_currentId, cavityheight, xpermeability, ypermeability, porosity);
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
    float cavityheight, xpermeability, ypermeability, porosity;
    bool result = getInputs(name, cavityheight, xpermeability, ypermeability, porosity);
    if (result)
    {
        hide();
        emit gotMaterial(name, cavityheight, xpermeability, ypermeability, porosity);
        close();
    }
    else
        QMessageBox::warning(this, ERROR_TITLE, INVALID_PARAM_ERROR);
}

VWindowCloth::~VWindowCloth()
{
    delete m_pDatabaseCloth;
    m_pPartValidator->deleteLater();
    #ifdef DEBUG_MODE
        qInfo() << "VWindowCloth destroyed";
    #endif
}

VDatabaseInteractor* VWindowCloth::databaseInteractor()
{
    return m_pDatabaseCloth;
}

bool VWindowCloth::getInputs(QString &name, float &cavityheight, float &xpermeability,
                             float &ypermeability, float &porosity)
{
    name = ui->nameEdit->text();
    if (name.isEmpty())
        return false;
    bool ok;
    cavityheight = m_ui.cavityHeightEdit->text().replace(',', '.').toFloat(&ok);
    if (!ok)
        return false;
    xpermeability = m_ui.xPermeabilityEdit->text().replace(',', '.').toFloat(&ok);
    if (!ok)
        return false;
    ypermeability = m_ui.yPermeabilityEdit->text().replace(',', '.').toFloat(&ok);
    if (!ok)
        return false;
    QString porosityStr = m_ui.porosityEdit->text().replace(',', '.');
    int pos = 0;
    QValidator::State state = m_ui.porosityEdit->validator()->validate(porosityStr, pos);
    if (state == QValidator::Acceptable)
        porosity = porosityStr.toDouble(&ok);
    else
        return false;
    return ok;
}
