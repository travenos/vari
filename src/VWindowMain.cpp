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
const QString VWindowMain::INVALID_PARAM_ERROR("Введены некорректные параметры");
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
    m_pWindowResin(nullptr),
    m_pTemperatureValidator(new QDoubleValidator),
    m_pPressureValidator(new QDoubleValidator),
    m_pDiameterValidator(new QDoubleValidator)
{
    ui->setupUi(this);
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,0);
    ui->layerParamBox->setVisible(false);
    m_pFacade.reset(new VSimulationFacade(ui->viewerWidget));
    connectSimulationSignals();
    setupValidators();
}

void VWindowMain::connectSimulationSignals()
{
    connect(m_pFacade.get(), SIGNAL(layerRemoved(unsigned int)),
            this, SLOT(m_on_layer_removed(unsigned int)));
    connect(m_pFacade.get(), SIGNAL(materialChanged(unsigned int)),
            this, SLOT(m_on_material_changed(unsigned int)));
    connect(m_pFacade.get(), SIGNAL(layerEnabled(unsigned int, bool)),
            this, SLOT(m_on_layer_enabled(unsigned int, bool)));
    connect(m_pFacade.get(), SIGNAL(injectionPointSet()),
            this, SLOT(m_on_injection_point_set()));
    connect(m_pFacade.get(), SIGNAL(vacuumPointSet()),
            this, SLOT(m_on_vacuum_point_set()));
    connect(m_pFacade.get(), SIGNAL(simulationStarted()),
            this, SLOT(m_on_simutation_started()));
    connect(m_pFacade.get(), SIGNAL(simulationPaused()),
            this, SLOT(m_on_simutation_paused()));
    connect(m_pFacade.get(), SIGNAL(simulationStopped()),
            this, SLOT(m_on_simutation_stopped()));
}

void VWindowMain::setupValidators()
{
    m_pTemperatureValidator->setLocale(QLocale::C);
    m_pPressureValidator->setLocale(QLocale::C);
    m_pDiameterValidator->setLocale(QLocale::C);
    m_pTemperatureValidator->setBottom(-VSimulationParametres::KELVINS_IN_0_CELSIUS);
    m_pPressureValidator->setBottom(0);
    m_pDiameterValidator->setBottom(0);
    ui->temperatureEdit->setValidator(m_pTemperatureValidator);
    ui->injectionPressureEdit->setValidator(m_pPressureValidator);
    ui->vacuumPressureEdit->setValidator(m_pPressureValidator);
    ui->injectionDiameterEdit->setValidator(m_pDiameterValidator);
    ui->vacuumDiameterEdit->setValidator(m_pDiameterValidator);
}

VWindowMain::~VWindowMain()
{
    delete ui;
    deleteWindowLayer();
    deleteWindowResin();
    deleteWindowCloth();
    VSqlDatabase::deleteInstance();
    delete m_pPressureValidator;
    delete m_pTemperatureValidator;
    delete m_pDiameterValidator;
    #ifdef DEBUG_MODE
        qInfo() << "VWindowMain destroyed";
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

void VWindowMain::addLayerFromFile(const VCloth& material,const QString& filename,
                                   VLayerAbstractBuilder::VUnit units)
{
    try
    {
        m_pFacade->newLayerFromFile(material, filename, units);
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

void VWindowMain::addLayerFromPolygon(const VCloth& material, const VPolygon& polygon,
                                      VLayerAbstractBuilder::VUnit units)
{
    //TODO
}

void VWindowMain::showWindowLayer()
{
    if (m_pWindowLayer == nullptr)
    {
        m_pWindowLayer = new VWindowLayer(this);
        connect(m_pWindowLayer,
                SIGNAL(creationFromFileAvailable(const VCloth&,const QString&, VLayerAbstractBuilder::VUnit)),
                this,
                SLOT(m_on_layer_creation_from_file_available(const VCloth&,const QString&,VLayerAbstractBuilder::VUnit)));
        connect(m_pWindowLayer,
                SIGNAL(creationManualAvailable(const VCloth&,const VPolygon&, VLayerAbstractBuilder::VUnit)),
                this,
                SLOT(m_on_layer_creation_manual_available(const VCloth&,const VPolygon&, VLayerAbstractBuilder::VUnit)));
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
        m_pFacade->removeLayer(layer);
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
    }
}

void VWindowMain::setResin(const QString & name , float viscosity, float tempcoef)
{
    m_pFacade->setDefaultViscosity(viscosity);
    m_pFacade->setTempcoef(tempcoef);
    ui->resinInfoLabel->setText(RESIN_INFO_TEXT.arg(name).arg(viscosity)
                                         .arg(tempcoef));
}

void VWindowMain::removeLayerFromList(int layer)
{
    if(layer < ui->layersListWidget->count())
    {
        delete ui->layersListWidget->item(layer);
        if (layer > 0)
            ui->layersListWidget->setCurrentRow(layer - 1);
    }
}

void VWindowMain::updateLayerMaterialInfo(int layer)
{
    if ( layer == ui->layersListWidget->currentRow())
    {
        VCloth::const_ptr cloth = m_pFacade->getMaterial(layer);
        ui->layersListWidget->currentItem()->setText(cloth->name);
        ui->layerInfoLabel->setText(CLOTH_INFO_TEXT.arg(cloth->name).arg(cloth->cavityHeight)
                                             .arg(cloth->permeability).arg(cloth->porosity));
    }
}

void VWindowMain::markLayerAsEnabled(int layer, bool enable)
{
    if(layer < ui->layersListWidget->count())
    {
        QColor textColor = enable ? QColor::fromRgb(0, 0, 0) : QColor::fromRgb(127, 127, 127);
        ui->layersListWidget->item(layer)->setTextColor(textColor);
    }
}

void VWindowMain::injectionPointSelectionResult()
{
    ui->injectionPlaceButton->setChecked(false);
}

void VWindowMain::startInjectionPointSelection()
{
    double diameter;
    bool ok = readNumber(ui->injectionDiameterEdit, diameter);
    if (ok)
        m_pFacade->waitForInjectionPointSelection(diameter);
    else
    {
        ui->injectionPlaceButton->setChecked(false);
        QMessageBox::warning(this, QStringLiteral("Error"), INVALID_PARAM_ERROR);
    }
}

void VWindowMain::cancelInjectionPointSelection()
{
    m_pFacade->cancelWaitingForInjectionPointSelection();
}

void VWindowMain::startVacuumPointSelection()
{
    double diameter;
    bool ok = readNumber(ui->vacuumDiameterEdit, diameter);
    if (ok)
        m_pFacade->waitForVacuumPointSelection(diameter);
    else
    {
        ui->vacuumPlaceButton->setChecked(false);
        QMessageBox::warning(this, QStringLiteral("Error"), INVALID_PARAM_ERROR);
    }
}

void VWindowMain::cancelVacuumPointSelection()
{
    m_pFacade->cancelWaitingForVacuumPointSelection();
}

void VWindowMain::vacuumPointSelectionResult()
{
    ui->vacuumPlaceButton->setChecked(false);
}

void VWindowMain::startSimulation()
{
    m_pFacade->startSimulation();
}

void VWindowMain::stopSimulation()
{
    m_pFacade->stopSimulation();
}

void VWindowMain::pauseSimulation()
{
    m_pFacade->pauseSimulation();
}

void VWindowMain::resetSimulationState()
{
    m_pFacade->resetSimulation();
}

void VWindowMain::saveTemperature()
{
    double temperature;
    bool ok = readNumber(ui->temperatureEdit, temperature);
    if (ok)
        m_pFacade->setTemperature(temperature);
    else
        QMessageBox::warning(this, QStringLiteral("Error"), INVALID_PARAM_ERROR);
}

void VWindowMain::saveInjectionPressure()
{
    double injectionPressure;
    bool ok = readNumber(ui->injectionPressureEdit, injectionPressure);
    if (ok)
        m_pFacade->setInjectionPressure(injectionPressure);
    else
        QMessageBox::warning(this, QStringLiteral("Error"), INVALID_PARAM_ERROR);
}

void VWindowMain::saveVacuumPressure()
{
    double vacuumPressure;
    bool ok = readNumber(ui->vacuumPressureEdit, vacuumPressure);
    if (ok)
        m_pFacade->setVacuumPressure(vacuumPressure);
    else
        QMessageBox::warning(this, QStringLiteral("Error"), INVALID_PARAM_ERROR);
}

void VWindowMain::showTemperature()
{
    double temperature = m_pFacade->getParametres()->getTemperature();
    ui->temperatureEdit->setText(QString::number(temperature));
}

void VWindowMain::showInjectionPressure()
{
    double injectionPressure = m_pFacade->getParametres()->getInjectionPressure();
    ui->injectionPressureEdit->setText(QString::number(injectionPressure));
}

void VWindowMain::showInjectionDiameter()
{
    double injectionDiameter = m_pFacade->getParametres()->getInjectionDiameter();
    ui->injectionDiameterEdit->setText(QString::number(injectionDiameter));
}

void VWindowMain::showInjectionPoint()
{
    m_pFacade->showInjectionPoint();
}

void VWindowMain::showVacuumPressure()
{
    double vacuumPressure = m_pFacade->getParametres()->getVacuumPressure();
    ui->vacuumPressureEdit->setText(QString::number(vacuumPressure));
}

void VWindowMain::showVacuumDiameter()
{
    double vacuumDiameter = m_pFacade->getParametres()->getVacuumDiameter();
    ui->vacuumDiameterEdit->setText(QString::number(vacuumDiameter));
}
void VWindowMain::showVacuumPoint()
{
    m_pFacade->showVacuumPoint();
}

bool VWindowMain::readNumber(const QLineEdit * lineEdit, double &output) const
{
    bool ok;
    QString numberStr = lineEdit->text().replace(',', '.');
    int pos = 0;
    QValidator::State state = lineEdit->validator()->validate(numberStr, pos);
    if (state == QValidator::Acceptable)
        output = numberStr.toDouble(&ok);
    else
        return false;
    return ok;
}

void VWindowMain::simulationStartResult()
{
    ui->actionStart->setEnabled(false);
    ui->actionPause->setEnabled(true);
    ui->actionStop->setEnabled(true);
}

void VWindowMain::simulationPauseResult()
{
    ui->actionStart->setEnabled(true);
    ui->actionPause->setEnabled(false);
    ui->actionStop->setEnabled(true);
}

void VWindowMain::simulationStopResult()
{
    ui->actionStart->setEnabled(true);
    ui->actionPause->setEnabled(false);
    ui->actionStop->setEnabled(false);
}

/**
 * Slots
 */

void VWindowMain::m_on_layer_creation_from_file_available(const VCloth& material, const QString& filename,
                                                          VLayerAbstractBuilder::VUnit units)
{
    addLayerFromFile(material, filename, units);
}

void VWindowMain::m_on_layer_creation_manual_available(const VCloth& material,const VPolygon& polygon,
                                                       VLayerAbstractBuilder::VUnit units)
{
    addLayerFromPolygon(material, polygon, units);
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

void VWindowMain::m_on_layer_removed(unsigned int layer)
{
    removeLayerFromList(layer);
}

void VWindowMain::m_on_material_changed(unsigned int layer)
{
    updateLayerMaterialInfo(layer);
}

void VWindowMain::m_on_layer_enabled(unsigned int layer, bool enable)
{
    markLayerAsEnabled(layer, enable);
}

void VWindowMain::m_on_injection_point_set()
{
    injectionPointSelectionResult();
    showInjectionPoint();
}

void VWindowMain::m_on_vacuum_point_set()
{
    vacuumPointSelectionResult();
    showVacuumPoint();
}

void VWindowMain::m_on_simutation_started()
{
    simulationStartResult();
}

void VWindowMain::m_on_simutation_paused()
{
    simulationPauseResult();
}

void VWindowMain::m_on_simutation_stopped()
{
    simulationStopResult();
}

void VWindowMain::on_injectionPlaceButton_clicked(bool checked)
{
    if (checked)
        startInjectionPointSelection();
    else
        cancelInjectionPointSelection();
}

void VWindowMain::on_vacuumPlaceButton_clicked(bool checked)
{
    if (checked)
        startVacuumPointSelection();
    else
        cancelVacuumPointSelection();
}

void VWindowMain::on_actionStart_triggered()
{
    startSimulation();
}

void VWindowMain::on_actionStop_triggered()
{
    stopSimulation();
}

void VWindowMain::on_actionReset_triggered()
{
    resetSimulationState();
}

void VWindowMain::on_resetTemperatureButton_clicked()
{
    showTemperature();
}

void VWindowMain::on_saveTemperatureButton_clicked()
{
    saveTemperature();
}

void VWindowMain::on_resetInjectionPressureButton_clicked()
{
    showInjectionPressure();
}

void VWindowMain::on_saveInjectionPressureButton_clicked()
{
    saveInjectionPressure();
}

void VWindowMain::on_resetInjectionDiamterButton_clicked()
{
    showInjectionDiameter();
}

void VWindowMain::on_showInjectionPlace_clicked()
{
    showInjectionPoint();
}

void VWindowMain::on_resetVacuumPressureButton_clicked()
{
    showVacuumPressure();
}

void VWindowMain::on_saveVacuumPressureButton_clicked()
{
    saveVacuumPressure();
}

void VWindowMain::on_resetVacuumDiameterButton_clicked()
{
    showVacuumDiameter();
}

void VWindowMain::on_showVacuumPlaceButton_clicked()
{
    showVacuumPoint();
}

void VWindowMain::on_actionPause_triggered()
{
    pauseSimulation();
}
