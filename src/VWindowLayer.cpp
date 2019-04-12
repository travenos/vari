/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QSettings>

#include "VWindowLayer.h"
#include "ui_VWindowLayer.h"
#include "VWindowCloth.h"
#include "VWindowPolygon.h"

const QString VWindowLayer::NO_MATERIAL_TEXT("Материал слоя не задан");
const QString VWindowLayer::NO_GEOMETRY_TEXT("Форма слоя не задана");
const QString VWindowLayer::COLOR_DIALOG_TITLE("Выбор цвета по умолчанию для слоя");
const QString VWindowLayer::FILE_DIALOG_TITLE("Импорт формы слоя");
const QString VWindowLayer::FILE_DIALOG_FORMATS("Поддерживаемые форматы (*.msh *.db *.ansys);;"
                                                "Файлы gmsh(*.msh);;"
                                                "Файлы Ansys (*.db *.ansys);;"
                                                "Все файлы (*)");
const QString VWindowLayer::GEOMETRY_FROM_FILE_TEXT("Форма слоя будет импортирована из файла:\n%1");
const QString VWindowLayer::GEOMETRY_MANUAL_TEXT("Форма слоя задана пользователем");
const QString VWindowLayer::MATERIAL_NAME_TEXT("Выбран материал: %1");
const QString VWindowLayer::MATERIAL_INFO_TEXT("<html><head/><body>"
                                               "Материал: &quot;%1&quot;<br>"
                                               "Толщина: %2 м<br>"
                                               "Проницаемость: %3 м<span style=\" vertical-align:super;\">2</span><br>"
                                               "Пористость: %4"
                                               "</body></html>");

const QColor VWindowLayer::DEFAULT_COLOR = QColor(255, 172, 172);

VWindowLayer::VWindowLayer(QWidget *parent, std::shared_ptr<const VTable> p_table) :
    QMainWindow(parent),
    ui(new Ui::VWindowLayer),
    m_selectedMaterial(false),
    m_selectedFile(false),
    m_createdGeometry(false),
    m_pTable(p_table)
{
    ui->setupUi(this);
    reset();
    loadSavedParameters();
    showColor();
}

VWindowLayer::~VWindowLayer()
{
    m_pWindowCloth.reset();
    m_pWindowPolygon.reset();
    saveParameters();
    delete ui;
    #ifdef DEBUG_MODE
        qInfo() << "VWindowLayer destroyed";
    #endif
}

void VWindowLayer::reset()
{
    ui->materialStatusLabel->setStyleSheet(QStringLiteral("QLabel { color : red; }"));
    ui->materialStatusLabel->setText(NO_MATERIAL_TEXT);
    ui->geometryStatusLabel->setStyleSheet(QStringLiteral("QLabel { color : red; }"));
    ui->geometryStatusLabel->setText(NO_GEOMETRY_TEXT);
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(false);
    ui->mmCheckBox->setEnabled(false);
    m_selectedMaterial = false;
    m_selectedFile = false;
    m_createdGeometry = false;
}

void VWindowLayer::loadSavedParameters()
{
    QSettings settings;
    bool mm_selected;
    mm_selected = settings.value(QStringLiteral("import/sizeInMM"), false).toBool();
    m_lastDir = settings.value(QStringLiteral("import/lastDir"), QDir::homePath()).toString();
    ui->mmCheckBox->setChecked(mm_selected);
    QVariant color = settings.value(QStringLiteral("import/color"), DEFAULT_COLOR);
    m_material.baseColor = color.value<QColor>();
}

void VWindowLayer::setTable(const std::shared_ptr<const VTable> & p_table)
{
    m_pTable = p_table;
}

void VWindowLayer::saveParameters() const
{
    QSettings settings;
    bool mm_selected = ui->mmCheckBox->isChecked();
    settings.setValue(QStringLiteral("import/sizeInMM"), mm_selected);
    settings.setValue(QStringLiteral("import/lastDir"), m_lastDir);
    settings.setValue(QStringLiteral("import/color"), m_material.baseColor);
    settings.sync();
}

void VWindowLayer::accept()
{
    hide();
    VLayerAbstractBuilder::VUnit units;
    if (ui->mmCheckBox->isChecked())
        units = VLayerAbstractBuilder::MM;
    else
        units = VLayerAbstractBuilder::M;
    if (m_selectedMaterial && m_selectedFile)
        emit creationFromFileAvailable(m_material, m_filename, units);
    else if (m_selectedMaterial && m_createdGeometry)
        emit creationManualAvailable(m_material, m_polygon, m_characteristicLength);
    reset();
    close();
}

void VWindowLayer::reject()
{
    hide();
    reset();
    emit creationCanceled();
    close();
}

void VWindowLayer::updateButtonsStates()
{
    bool enable = (m_selectedMaterial && (m_selectedFile || m_createdGeometry));
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(enable);
    ui->mmCheckBox->setEnabled(m_selectedFile);
}

void VWindowLayer::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, FILE_DIALOG_TITLE, m_lastDir,
                                                    FILE_DIALOG_FORMATS);
    if (!fileName.isEmpty() && QFile::exists(fileName))
    {
        m_filename = fileName;
        m_lastDir = QFileInfo(m_filename).absolutePath();
        ui->geometryStatusLabel->setText(GEOMETRY_FROM_FILE_TEXT.arg(fileName));
        ui->geometryStatusLabel->setStyleSheet(QStringLiteral("QLabel { color : black; }"));
        m_selectedFile = true;
        m_createdGeometry = false;
        updateButtonsStates();
    }
}

void VWindowLayer::showWindowPolygon()
{
    if(!m_pWindowPolygon)
    {
        m_pWindowPolygon.reset(new VWindowPolygon(this, m_pTable));
        connect(m_pWindowPolygon.get(), SIGNAL(polygonAvailable(const QPolygonF &, double)),
                this, SLOT(m_on_got_polygon(const QPolygonF &, double)));
    }
    else
    {
        m_pWindowPolygon->setTable(m_pTable);
    }
    m_pWindowPolygon->show();
    m_pWindowPolygon->activateWindow();
}

void VWindowLayer::showWindowCloth()
{
    if(!m_pWindowCloth)
    {
        m_pWindowCloth.reset(new VWindowCloth(this));
        connect(m_pWindowCloth.get(), SIGNAL(gotMaterial(const QString &, float, float, float)),
                this, SLOT(m_on_got_material(const QString &, float, float, float)));
    }
    m_pWindowCloth->show();
    m_pWindowCloth->activateWindow();
}

void VWindowLayer::selectColor()
{
    QColor color = QColorDialog::getColor(DEFAULT_COLOR, this, COLOR_DIALOG_TITLE);

    if (color.isValid())
    {
        m_material.baseColor = color;
        showColor();
    }
}

void VWindowLayer::showColor()
{
    QColor color = m_material.baseColor;
    QPalette palette = ui->colorButton->palette();
    palette.setColor(QPalette::Button, color);
    palette.setColor(QPalette::Light, color);
    palette.setColor(QPalette::Dark, color);
    ui->colorButton->setPalette(palette);
}

void VWindowLayer::closeEvent(QCloseEvent *)
{
    emit windowClosed();
}

void VWindowLayer::m_on_got_material(const QString &name, float cavityheight, float permeability, float porosity)
{
    m_material.name = name;
    m_material.cavityHeight = cavityheight;
    m_material.permeability = permeability;
    m_material.porosity = porosity;
    ui->materialStatusLabel->setText(MATERIAL_INFO_TEXT.arg(name).arg(cavityheight)
                                         .arg(permeability).arg(porosity));
    ui->materialStatusLabel->setStyleSheet(QStringLiteral("QLabel { color : black; }"));
    m_selectedMaterial = true;
    updateButtonsStates();
}

void VWindowLayer::m_on_got_polygon(const QPolygonF &polygon, double characteristicLength)
{
    m_polygon = polygon;
    m_characteristicLength = characteristicLength;
    if (polygon.size() >= VWindowPolygon::MIN_POLYGON_SIZE)
    {
        ui->geometryStatusLabel->setText(GEOMETRY_MANUAL_TEXT);
        ui->geometryStatusLabel->setStyleSheet(QStringLiteral("QLabel { color : black; }"));
        m_selectedFile = false;
        m_createdGeometry = true;
        updateButtonsStates();
    }
}

void VWindowLayer::on_buttonBox_rejected()
{
    reject();
}

void VWindowLayer::on_buttonBox_accepted()
{
    accept();
}

void VWindowLayer::on_createFromFileButton_clicked()
{
    openFile();
}

void VWindowLayer::on_chooseMaterialButton_clicked()
{
    showWindowCloth();
}

void VWindowLayer::on_colorButton_clicked()
{
    selectColor();
}

void VWindowLayer::on_createManualButton_clicked()
{
    showWindowPolygon();
}
