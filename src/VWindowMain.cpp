#ifdef DEBUG_MODE
#include <QDebug>
#endif
#include <QMessageBox>
#include "VWindowMain.h"
#include "ui_VWindowMain.h"
#include "sim/VSimulationFacade.h"
#include "sim/VExceptions.h"
#include "VWindowCloth.h"
#include "VWindowResin.h"
#include "VWindowLayer.h"

const QString VWindowMain::IMPORT_FROM_FILE_ERROR("Ошибка загрузки слоя из файла");
const QString VWindowMain::IMPORT_WHEN_SIMULATING_ERROR("Невозможно импортировать во время симуляции");

VWindowMain::VWindowMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VWindowMain),
    m_pWindowLayer(nullptr)
{
    ui->setupUi(this);
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,0);
    m_pFacade.reset(new VSimulationFacade(ui->viewerWidget));
}

VWindowMain::~VWindowMain()
{
    delete ui;
    VSqlDatabase::deleteInstance();
    deleteWindowLayer();
    #ifdef DEBUG_MODE
        qDebug() << "VWindowMain destroyed";
    #endif
}

void VWindowMain::deleteWindowLayer()
{
    if (m_pWindowLayer != nullptr)
    {
        m_pWindowLayer->close();
        m_pWindowLayer->deleteLater();
        m_pWindowLayer = nullptr;
    }
}

void VWindowMain::addLayerFromFile(const VCloth& material,const QString& filename)
{
    try
    {
        m_pFacade->newLayerFromFile(material, filename);
    }
    catch (VImportException)
    {
        QMessageBox::warning(this, QStringLiteral("Error"), IMPORT_FROM_FILE_ERROR);
    }
    catch (VSimulatorException)
    {
        QMessageBox::warning(this, QStringLiteral("Error"), IMPORT_WHEN_SIMULATING_ERROR);
    }
}

void VWindowMain::addLayerFromPolygon(const VCloth& material,const VPolygon& polygon)
{
    //TODO
}

void VWindowMain::showWindowLayer()
{
    if (m_pWindowLayer == nullptr)
    {
        m_pWindowLayer = new VWindowLayer(this);
        connect(m_pWindowLayer, SIGNAL(creationFromFileAvailable(const VCloth&,const QString&)),
                this, SLOT(m_on_layer_creation_from_file_available(const VCloth&,const QString&)));
        connect(m_pWindowLayer, SIGNAL(creationManualAvailable(const VCloth&,const VPolygon&)),
                this, SLOT(m_on_layer_creation_manual_available(const VCloth&,const VPolygon&)));
        connect(m_pWindowLayer,SIGNAL(windowClosed()), this, SLOT(m_on_layer_window_closed()));
    }
    m_pWindowLayer->show();
    m_pWindowLayer->activateWindow();
}

void VWindowMain::m_on_layer_creation_from_file_available(const VCloth& material, const QString& filename)
{
    addLayerFromFile(material, filename);
}

void VWindowMain::m_on_layer_creation_manual_available(const VCloth& material,const VPolygon& polygon)
{
    addLayerFromPolygon(material, polygon);
}

void VWindowMain::m_on_layer_window_closed()
{
    deleteWindowLayer();
}

void VWindowMain::on_addLayerButton_clicked()
{
    showWindowLayer();
}
