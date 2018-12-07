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
const QString VWindowMain::ASK_FOR_REMOVE("Вы уверены, что хотите удалить слой?");
const QString VWindowMain::CLOTH_INFO_TEXT("<html><head/><body>"
                                               "Материал: &quot;%1&quot;<br>"
                                               "Толщина: %2 м<br>"
                                               "Проницаемость: %3 м<span style=\" vertical-align:super;\">2</span><br>"
                                               "Пористость: %4"
                                               "</body></html>");
const QString VWindowMain::RESIN_INFO_TEXT("<html><head/><body>"
                                               "Материал: &quot;%1&quot;<br>"
                                               "Вязкость при 25 °C: %2 Па·с<br>"
                                               "Температурный коэффициент: %3 Па·с"
                                               "</body></html>");

VWindowMain::VWindowMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VWindowMain),
    m_pWindowLayer(nullptr),
    m_pWindowCloth(nullptr),
    m_pWindowResin(nullptr)
{
    ui->setupUi(this);
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,0);
    ui->layerParamBox->setVisible(false);
    m_pFacade.reset(new VSimulationFacade(ui->viewerWidget));
}

VWindowMain::~VWindowMain()
{
    delete ui;
    deleteWindowLayer();
    deleteWindowResin();
    deleteWindowCloth();
    VSqlDatabase::deleteInstance();
    #ifdef DEBUG_MODE
        qDebug() << "VWindowMain destroyed";
    #endif
}

void VWindowMain::deleteWindowLayer()
{
    if (m_pWindowLayer != nullptr)
    {
        m_pWindowLayer->deleteLater();
        m_pWindowLayer = nullptr;
    }
}

void VWindowMain::deleteWindowCloth()
{
    if (m_pWindowCloth != nullptr)
    {
        m_pWindowCloth->deleteLater();
        m_pWindowCloth = nullptr;
    }
}

void VWindowMain::deleteWindowResin()
{
    if (m_pWindowResin != nullptr)
    {
        m_pWindowResin->deleteLater();
        m_pWindowResin = nullptr;
    }
}

void VWindowMain::addLayerFromFile(const VCloth& material,const QString& filename)
{
    try
    {
        m_pFacade->newLayerFromFile(material, filename);
        ui->layersListWidget->addItem(material.name);
        ui->layersListWidget->setCurrentRow(ui->layersListWidget->count() - 1);
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

void VWindowMain::selectLayer()
{
    if (ui->layersListWidget->currentIndex().isValid())
    {
        unsigned int layer = ui->layersListWidget->currentRow();
        VCloth::const_ptr cloth = m_pFacade->getMaterial(layer);
        bool visible = m_pFacade->isLayerVisible(layer);
        bool enabled = m_pFacade->isLayerEnabled(layer);        
        ui->layerParamBox->setVisible(true);
        ui->layerVisibleCheckBox->setChecked(visible);
        ui->layerEnableCheckBox->setChecked(enabled);
        ui->layerInfoLabel->setText(CLOTH_INFO_TEXT.arg(cloth->name).arg(cloth->cavityHeight)
                                             .arg(cloth->permeability).arg(cloth->porosity));
        //TODO show color
    }
    else
        ui->layerParamBox->setVisible(false);
}


void VWindowMain::enableLayer(bool enable)
{
    if (ui->layersListWidget->currentIndex().isValid())
    {
        unsigned int layer = ui->layersListWidget->currentRow();
        m_pFacade->enableLayer(layer, enable);
    }
}

void VWindowMain::setVisibleLayer(bool visible)
{
    if (ui->layersListWidget->currentIndex().isValid())
    {
        unsigned int layer = ui->layersListWidget->currentRow();
        m_pFacade->setVisible(layer, visible);
    }
}

void VWindowMain::removeLayer()
{
    if (ui->layersListWidget->currentIndex().isValid())
    {
        int layer = ui->layersListWidget->currentRow();
        delete ui->layersListWidget->currentItem();
        m_pFacade->removeLayer(layer);
        if (layer > 0)
            ui->layersListWidget->setCurrentRow(layer - 1);
    }
}

void VWindowMain::showWindowCloth()
{
    if (m_pWindowCloth == nullptr)
    {
        m_pWindowCloth = new VWindowCloth(this);
        connect(m_pWindowCloth, SIGNAL(gotMaterial(const QString &, float, float, float)),
                this, SLOT(m_on_got_cloth(const QString &, float, float, float)));
        connect(m_pWindowCloth,SIGNAL(windowClosed()), this, SLOT(m_on_cloth_window_closed()));
    }
    m_pWindowCloth->show();
    m_pWindowCloth->activateWindow();
}

void VWindowMain::showWindowResin()
{
    if (m_pWindowResin == nullptr)
    {
        m_pWindowResin = new VWindowResin(this);
        connect(m_pWindowResin, SIGNAL(gotMaterial(const QString &, float, float)),
                this, SLOT(m_on_got_resin(const QString &, float, float)));
        connect(m_pWindowResin,SIGNAL(windowClosed()), this, SLOT(m_on_resin_window_closed()));
    }
    m_pWindowResin->show();
    m_pWindowResin->activateWindow();
}

void VWindowMain::setCloth(const QString & name, float cavityheight, float permeability, float porosity)
{
    if (ui->layersListWidget->currentIndex().isValid())
    {
        int layer = ui->layersListWidget->currentRow();
        VCloth cloth;
        cloth.name = name;
        cloth.cavityHeight = cavityheight;
        cloth.permeability = permeability;
        cloth.porosity = porosity;
        m_pFacade->setMaterial(layer, cloth);
        ui->layersListWidget->currentItem()->setText(name);
        ui->layerInfoLabel->setText(CLOTH_INFO_TEXT.arg(name).arg(cavityheight)
                                             .arg(permeability).arg(porosity));
    }
}

void VWindowMain::setResin(const QString & name , float viscosity, float tempcoef)
{
    m_pFacade->setDefaultViscosity(viscosity);
    m_pFacade->setTempcoef(tempcoef);
    ui->resinInfoLabel->setText(RESIN_INFO_TEXT.arg(name).arg(viscosity)
                                         .arg(tempcoef));
}

void VWindowMain::m_on_layer_creation_from_file_available(const VCloth& material, const QString& filename)
{
    addLayerFromFile(material, filename);
}

void VWindowMain::m_on_layer_creation_manual_available(const VCloth& material,const VPolygon& polygon)
{
    addLayerFromPolygon(material, polygon);
}

void VWindowMain::m_on_got_cloth(const QString & name, float cavityheight, float permeability, float porosity)
{
    setCloth(name, cavityheight, permeability, porosity);
}

void VWindowMain::m_on_got_resin(const QString & name , float viscosity, float tempcoef)
{
    setResin(name, viscosity, tempcoef);
}

void VWindowMain::m_on_layer_window_closed()
{
    deleteWindowLayer();
}

void VWindowMain::m_on_cloth_window_closed()
{
    deleteWindowCloth();
}

void VWindowMain::m_on_resin_window_closed()
{
    deleteWindowResin();
}

void VWindowMain::on_addLayerButton_clicked()
{
    showWindowLayer();
}

void VWindowMain::on_layersListWidget_itemSelectionChanged()
{
    selectLayer();
}

void VWindowMain::on_layerEnableCheckBox_clicked(bool checked)
{
    enableLayer(checked);
}

void VWindowMain::on_layerVisibleCheckBox_clicked(bool checked)
{
    setVisibleLayer(checked);
}

void VWindowMain::on_layerRemoveButton_clicked()
{
    if (QMessageBox::question(this, QStringLiteral("Remove?"),
                              ASK_FOR_REMOVE, QMessageBox::Yes|QMessageBox::No )==QMessageBox::Yes)
    {
        removeLayer();
    }
}

void VWindowMain::on_selectMaterialClothButton_clicked()
{
    showWindowCloth();
}

void VWindowMain::on_selectMaterialResinButton_clicked()
{
    showWindowResin();
}
