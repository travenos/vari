#ifdef DEBUG_MODE
#include <QDebug>
#endif
#include <QMessageBox>
#include <QFileDialog>
#include "VWindowLayer.h"
#include "ui_VWindowLayer.h"
#include "VWindowCloth.h"

const QString VWindowLayer::NO_MATERIAL_TEXT("Материал слоя не задан");
const QString VWindowLayer::NO_GEOMETRY_TEXT("Форма слоя не задана");
const QString VWindowLayer::FILE_DIALOG_TITLE("Импорт формы слоя");
const QString VWindowLayer::FILE_DIALOG_FORMATS("Поддерживаемые форматы (*.msh *.iges *.igs *.db *.ansys);;"
                                                "Файлы gmsh(*.msh *.iges *.igs);;"
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
//const QColor VWindowLayer::DEFAULT_COLOR = QColor(255, 255, 255);

VWindowLayer::VWindowLayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VWindowLayer),
    m_color(DEFAULT_COLOR),
    m_selectedMaterial(false),
    m_selectedFile(false),
    m_createdGeometry(false)
{
    ui->setupUi(this);
    reset();
}

VWindowLayer::~VWindowLayer()
{
    delete ui;
    m_pWindowCloth.reset();
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
    m_material.baseColor = m_color;
    m_selectedMaterial = false;
    m_selectedFile = false;
    m_createdGeometry = false;
}

void VWindowLayer::accept()
{
    hide();
    VLayerAbstractBuilder::VUnit units;
    if (ui->mmRadioButton->isChecked())
        units = VLayerAbstractBuilder::MM;
    else
        units = VLayerAbstractBuilder::M;
    if (m_selectedMaterial && m_selectedFile)
        emit creationFromFileAvailable(m_material, m_filename, units);
    else if (m_selectedMaterial && m_createdGeometry)
        emit creationManualAvailable(m_material, m_polygon, units);
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

void VWindowLayer::tryToEnableAcceptButton()
{
    bool enable = (m_selectedMaterial && (m_selectedFile || m_createdGeometry));
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(enable);
}

void VWindowLayer::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, FILE_DIALOG_TITLE, QDir::homePath(),
                                                    FILE_DIALOG_FORMATS);
    if (!fileName.isEmpty() && QFile::exists(fileName))
    {
        m_filename = fileName;
        ui->geometryStatusLabel->setText(GEOMETRY_FROM_FILE_TEXT.arg(fileName));
        ui->geometryStatusLabel->setStyleSheet(QStringLiteral("QLabel { color : black; }"));
        m_selectedFile = true;
        m_createdGeometry = false;
        tryToEnableAcceptButton();
    }
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
    tryToEnableAcceptButton();
}

//TODO color chooser dialog

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
