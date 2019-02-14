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
#include <QTimeZone>

#include "VWindowMain.h"
#include "ui_VWindowMain.h"
#include "VWindowCloth.h"
#include "VWindowResin.h"
#include "VWindowLayer.h"
#include "VScreenShooter.h"
#include "sim/VSimulationFacade.h"
#include "sim/structures/VExceptions.h"

const QString VWindowMain::ERROR_TITLE("Ошибка");
const QString VWindowMain::IMPORT_FROM_FILE_ERROR("Ошибка загрузки из файла");
const QString VWindowMain::IMPORT_MANUAL_ERROR("Ошибка создания слоя");
const QString VWindowMain::EXPORT_TO_FILE_ERROR("Ошибка сохранения в файл");
const QString VWindowMain::IMPORT_WHEN_SIMULATING_ERROR("Невозможно импортировать во время симуляции");
const QString VWindowMain::INVALID_PARAM_ERROR("Введены некорректные параметры");
const QString VWindowMain::REMOVE_TITLE("Удалить?");
const QString VWindowMain::ASK_FOR_REMOVE("Вы уверены, что хотите удалить слой?");
const QString VWindowMain::CUT_TITLE("Обрезать?");
const QString VWindowMain::ASK_FOR_CUT("Вы уверены, что хотите выполнить обрезку слоя?");
const QString VWindowMain::TRANSFORM_TITLE("Переместить?");
const QString VWindowMain::ASK_FOR_TRANSFORM("Вы уверены, что хотите изменть положение слоя?");
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
const QString VWindowMain::MODEL_INFO_TEXT("Габариты: %1м x %2м x %3м;    "
                                           "Число узлов: %4;    "
                                           "Число треугольников: %5.");
const QString VWindowMain::OPEN_FILE_DIALOG_TITLE("Загрузка модели");
const QString VWindowMain::SAVE_FILE_DIALOG_TITLE("Сохранение модели");
const QString VWindowMain::FILE_DIALOG_FORMATS("Модели VARI (*.vari);;"
                                                "Все файлы (*)");

const QColor VWindowMain::ACTIVE_COLOR(0, 0, 0);
const QColor VWindowMain::INACTIVE_COLOR(255, 0, 0);
const QColor VWindowMain::INVISIBLE_COLOR(127, 127, 127);

const float VWindowMain::MAX_CUBE_SIDE = 0.0125f;

const QString VWindowMain::SLIDESHOW_DIR_DIALOG_TITLE("Путь сохранения слайдов");
const QString VWindowMain::VIDEO_FILE_DIALOG_TITLE("Файл, в который будет записано видео");
const Qt::WindowFlags VWindowMain::ON_TOP_FLAGS = (Qt::CustomizeWindowHint | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnBottomHint | Qt::WindowStaysOnTopHint | Qt::Dialog | Qt::WindowTitleHint);
const QString VWindowMain::BASE_VIDEO_DIR_NAME("_VARI_SLIDES_FOR_VIDEO_"); //TODO
const QString VWindowMain::VIDEO_DIR_PATH = QDir::cleanPath(QDir::tempPath() + QDir::separator() + VWindowMain::BASE_VIDEO_DIR_NAME); //TODO
const QString VWindowMain::BASE_SLIDESHOW_DIR_NAME("VARI_slideshow"); //TODO

VWindowMain::VWindowMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VWindowMain),
    m_pWindowLayer(nullptr),
    m_pWindowCloth(nullptr),
    m_pWindowResin(nullptr),
    m_pTemperatureValidator(new QDoubleValidator),
    m_pPressureValidator(new QDoubleValidator),
    m_pDiameterValidator(new QDoubleValidator),
    m_pSlideshowShooter(new VScreenShooter),
    m_pVideoShooter(new VScreenShooter)
{
    ui->setupUi(this);
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,0);
    ui->layerParamBox->setVisible(false);
    ui->modelInfoLabel->setVisible(false);
    m_pFacade.reset(new VSimulationFacade(ui->viewerWidget));
    connectSimulationSignals();
    setupValidators();
    m_pFacade->loadSavedParameters();
    showCubeSide();
    loadSizes();
    m_pSlideshowShooter->setWidget(m_pFacade->getGLWidget());
    m_pVideoShooter->setWidget(m_pFacade->getGLWidget());
    m_pVideoShooter->setDirName(VIDEO_DIR_PATH);

    loadShootersSettings();
}

void VWindowMain::connectSimulationSignals()
{
    connect(m_pFacade.get(), SIGNAL(layerRemoved(uint)),
            this, SLOT(m_on_layer_removed(uint)));
    connect(m_pFacade.get(), SIGNAL(materialChanged(uint)),
            this, SLOT(m_on_material_changed(uint)));
    connect(m_pFacade.get(), SIGNAL(layerEnabled(uint, bool)),
            this, SLOT(m_on_layer_enabled(uint, bool)));
    connect(m_pFacade.get(), SIGNAL(layerAdded()),
            this, SLOT(m_on_layer_added()));
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

    connect(m_pFacade.get(), SIGNAL(resinChanged()),
            this, SLOT(m_on_resin_changed()));
    connect(m_pFacade.get(), SIGNAL(layerVisibilityChanged(uint, bool)),
            this, SLOT(m_on_layer_visibility_changed(uint, bool)));
    connect(m_pFacade.get(), SIGNAL(injectionDiameterSet(double)),
            this, SLOT(m_on_injection_diameter_set(double)));
    connect(m_pFacade.get(), SIGNAL(vacuumDiameterSet(double)),
            this, SLOT(m_on_vacuum_diameter_set(double)));
    connect(m_pFacade.get(), SIGNAL(temperatureSet(double)),
            this, SLOT(m_on_temperature_set(double)));
    connect(m_pFacade.get(), SIGNAL(vacuumPressureSet(double)),
            this, SLOT(m_on_vacuum_pressure_set(double)));
    connect(m_pFacade.get(), SIGNAL(injectionPressureSet(double)),
            this, SLOT(m_on_injection_pressure_set(double)));
    connect(m_pFacade.get(), SIGNAL(timeLimitSet(double)),
            this, SLOT(m_on_time_limit_set(double)));
    connect(m_pFacade.get(), SIGNAL(timeLimitModeSwitched(bool)),
            this, SLOT(m_on_time_limit_mode_switched(bool)));
    connect(m_pFacade.get(), SIGNAL(canceledWaitingForInjectionPoint()),
            this, SLOT(m_on_canceled_waiting_for_injection_point()));
    connect(m_pFacade.get(), SIGNAL(canceledWaitingForVacuumPoint()),
            this, SLOT(m_on_canceled_waiing_for_vacuum_point()));
    connect(m_pFacade.get(), SIGNAL(layersCleared()),
            this, SLOT(m_on_layers_cleared()));
    connect(m_pFacade.get(), SIGNAL(modelLoaded()),
            this, SLOT(m_on_model_loaded()));
    connect(m_pFacade.get(), SIGNAL(selectionMade()),
            this, SLOT(m_on_selection_made()));
    connect(m_pFacade.get(), SIGNAL(gotTransformation()),
            this, SLOT(m_on_got_transformation()));
    connect(m_pFacade.get(), SIGNAL(configUpdated()),
            this, SLOT(m_on_model_config_updated()));
    connect(m_pFacade.get(), SIGNAL(selectionEnabled(bool)),
            this, SLOT(m_on_selection_enabled(bool)));
    connect(m_pFacade.get(), SIGNAL(cubeSideChanged(float)),
            this, SLOT(m_on_cube_side_changed(float)));

    connect(m_pSlideshowShooter.get(), SIGNAL(processStarted()), this, SLOT(m_on_slideshow_started()));
    connect(m_pSlideshowShooter.get(), SIGNAL(processStopped()), this, SLOT(m_on_slideshow_stopped()));
    connect(m_pSlideshowShooter.get(), SIGNAL(directoryChanged()), this, SLOT(m_on_slideshow_directory_changed()));
    connect(m_pSlideshowShooter.get(), SIGNAL(periodChanged()), this, SLOT(m_on_slideshow_period_changed()));
    connect(m_pVideoShooter.get(), SIGNAL(processStarted()), this, SLOT(m_on_video_started()));
    connect(m_pVideoShooter.get(), SIGNAL(processStopped()), this, SLOT(m_on_video_stopped()));
    connect(m_pVideoShooter.get(), SIGNAL(directoryChanged()), this, SLOT(m_on_video_directory_changed()));
    connect(m_pVideoShooter.get(), SIGNAL(periodChanged()), this, SLOT(m_on_video_period_changed()));
}

void VWindowMain::setupValidators()
{
    m_pTemperatureValidator->setLocale(QLocale::C);
    m_pPressureValidator->setLocale(QLocale::C);
    m_pDiameterValidator->setLocale(QLocale::C);
    m_pTemperatureValidator->setBottom(-VSimulationParameters::KELVINS_IN_0_CELSIUS);
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
    disconnect(m_pSlideshowShooter.get(), SIGNAL(processStarted()), this, SLOT(m_on_slideshow_started()));
    disconnect(m_pSlideshowShooter.get(), SIGNAL(processStopped()), this, SLOT(m_on_slideshow_stopped()));
    disconnect(m_pVideoShooter.get(), SIGNAL(processStarted()), this, SLOT(m_on_video_started()));
    disconnect(m_pVideoShooter.get(), SIGNAL(processStopped()), this, SLOT(m_on_video_stopped()));
    stopVideo();
    saveVideo();
    saveSootersSettings();
    m_pSlideshowShooter.reset();
    m_pVideoShooter.reset();
    m_pFacade->saveParameters();
    m_pFacade.reset();
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

void VWindowMain::closeEvent(QCloseEvent *)
{
    saveSizes();
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
    }
    catch (VImportException)
    {
        QMessageBox::warning(this, ERROR_TITLE, IMPORT_FROM_FILE_ERROR);
    }
    catch (VSimulatorException)
    {
        QMessageBox::warning(this, ERROR_TITLE, IMPORT_WHEN_SIMULATING_ERROR);
    }
}

void VWindowMain::addLayerFromPolygon(const VCloth& material, const QPolygonF& polygon,
                                      double characteristicLength)
{
    try
    {
        m_pFacade->newLayerFromPolygon(material, polygon, characteristicLength);
    }
    catch (VImportException)
    {
        QMessageBox::warning(this, ERROR_TITLE, IMPORT_MANUAL_ERROR);
    }
    catch (VSimulatorException)
    {
        QMessageBox::warning(this, ERROR_TITLE, IMPORT_WHEN_SIMULATING_ERROR);
    }
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
                SIGNAL(creationManualAvailable(const VCloth&,const QPolygonF&, double)),
                this,
                SLOT(m_on_layer_creation_manual_available(const VCloth&,const QPolygonF&, double)));
        connect(m_pWindowLayer,SIGNAL(windowClosed()), this, SLOT(m_on_layer_window_closed()));
    }
    m_pWindowLayer->show();
    m_pWindowLayer->activateWindow();
}

void VWindowMain::selectLayer()
{
    if (ui->layersListWidget->currentIndex().isValid()
            && ui->layersListWidget->currentRow() < int(m_pFacade->getLayersNumber()))
    {
        uint layer = ui->layersListWidget->currentRow();
        VCloth::const_ptr cloth = m_pFacade->getMaterial(layer);
        bool visible = m_pFacade->isLayerVisible(layer);
        bool enabled = m_pFacade->isLayerEnabled(layer);        
        ui->layerParamBox->setVisible(true);
        ui->layerVisibleCheckBox->setChecked(visible);
        ui->layerEnableCheckBox->setChecked(enabled);
        ui->layerInfoLabel->setText(CLOTH_INFO_TEXT.arg(cloth->name).arg(cloth->cavityHeight)
                                             .arg(cloth->permeability).arg(cloth->porosity));
        showColor(cloth->baseColor);
    }
    else
        ui->layerParamBox->setVisible(false);
    if (ui->layerCutButton->isChecked())
        cancelCuttingLayer();
}

void VWindowMain::showColor(const QColor& color)
{
    QPalette palette = ui->layerColorButton->palette();
    palette.setColor(QPalette::Button, color);
    palette.setColor(QPalette::Light, color);
    palette.setColor(QPalette::Dark, color);
    ui->layerColorButton->setPalette(palette);
}

void VWindowMain::enableLayer(bool enable)
{
    if (ui->layersListWidget->currentIndex().isValid())
    {
        uint layer = ui->layersListWidget->currentRow();
        m_pFacade->enableLayer(layer, enable);
    }
}

void VWindowMain::setVisibleLayer(bool visible)
{
    if (ui->layersListWidget->currentIndex().isValid())
    {
        uint layer = ui->layersListWidget->currentRow();
        m_pFacade->setVisible(layer, visible);
    }
}

void VWindowMain::enableTimeLimitMode(bool checked)
{
    m_pFacade->setTimeLimitMode(checked);
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
        VCloth cloth = *(m_pFacade->getMaterial(layer));
        cloth.cavityHeight = cavityheight;
        cloth.permeability = permeability;
        cloth.porosity = porosity;
        cloth.name = name;
        m_pFacade->setMaterial(layer, cloth);
    }
}

void VWindowMain::selectColor()
{
    if (ui->layersListWidget->currentIndex().isValid())
    {
        int layer = ui->layersListWidget->currentRow();
        VCloth cloth = *(m_pFacade->getMaterial(layer));
        QColor color = QColorDialog::getColor(cloth.baseColor, this);

        if (color.isValid())
        {
            cloth.baseColor = color;
            m_pFacade->setMaterial(layer, cloth);
        }
    }
}

void VWindowMain::setResin(const QString & name , float viscosity, float tempcoef)
{
    VResin resin;
    resin.defaultViscosity = viscosity;
    resin.tempcoef = tempcoef;
    resin.name = name;
    m_pFacade->setResin(resin);
}

void VWindowMain::removeLayerFromList(int layer)
{
    if(layer < ui->layersListWidget->count())
    {
        bool reselectFlag = false;
        if (layer == ui->layersListWidget->currentRow())
        {
            if(layer > 0)
                ui->layersListWidget->setCurrentRow(layer - 1);
            else
                reselectFlag = true;
        }
        delete ui->layersListWidget->item(layer);
        if (reselectFlag)
            selectLayer();
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
        showColor(cloth->baseColor);
    }
}

void VWindowMain::updateResinInfo()
{
    VResin resin = m_pFacade->getParameters().getResin();
    ui->resinInfoLabel->setText(RESIN_INFO_TEXT.arg(resin.name).arg(resin.defaultViscosity)
                                         .arg(resin.tempcoef));
}

void VWindowMain::markLayerAsEnabled(int layer, bool enable)
{
    if(layer < ui->layersListWidget->count())
    {
        QColor textColor;
        if (!m_pFacade->isLayerEnabled(layer))
            textColor = INACTIVE_COLOR;
        else if (!m_pFacade->isLayerVisible(layer))
            textColor = INVISIBLE_COLOR;
        else
            textColor = ACTIVE_COLOR;
        ui->layersListWidget->item(layer)->setTextColor(textColor);
        if (ui->layersListWidget->currentRow() == layer)
            ui->layerEnableCheckBox->setChecked(enable);
    }
}

void VWindowMain::markLayerAsVisible(int layer, bool visible)
{
    if(layer < ui->layersListWidget->count())
    {
        QColor textColor;
        if (!m_pFacade->isLayerEnabled(layer))
            textColor = INACTIVE_COLOR;
        else if (!m_pFacade->isLayerVisible(layer))
            textColor = INVISIBLE_COLOR;
        else
            textColor = ACTIVE_COLOR;
        ui->layersListWidget->item(layer)->setTextColor(textColor);
        if (ui->layersListWidget->currentRow() == layer)
            ui->layerVisibleCheckBox->setChecked(visible);
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
        QMessageBox::warning(this, ERROR_TITLE, INVALID_PARAM_ERROR);
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
        QMessageBox::warning(this, ERROR_TITLE, INVALID_PARAM_ERROR);
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
    resetAllInputs();
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
    resetAllInputs();
}

void VWindowMain::saveTemperature()
{
    double temperature;
    bool ok = readNumber(ui->temperatureEdit, temperature);
    if (ok)
        m_pFacade->setTemperature(temperature);
    else
        QMessageBox::warning(this, ERROR_TITLE, INVALID_PARAM_ERROR);
}

void VWindowMain::saveInjectionPressure()
{
    double injectionPressure;
    bool ok = readNumber(ui->injectionPressureEdit, injectionPressure);
    if (ok)
        m_pFacade->setInjectionPressure(injectionPressure);
    else
        QMessageBox::warning(this, ERROR_TITLE, INVALID_PARAM_ERROR);
}

void VWindowMain::saveVacuumPressure()
{
    double vacuumPressure;
    bool ok = readNumber(ui->vacuumPressureEdit, vacuumPressure);
    if (ok)
        m_pFacade->setVacuumPressure(vacuumPressure);
    else
        QMessageBox::warning(this, ERROR_TITLE, INVALID_PARAM_ERROR);
}

void VWindowMain::saveTimeLimit()
{
    double time = QTime(0,0).secsTo(ui->timeEdit->time());
    m_pFacade->setTimeLimit(time);
}

void VWindowMain::showTemperature()
{
    double temperature = m_pFacade->getParameters().getTemperature();
    ui->temperatureEdit->setText(QString::number(temperature));
    ui->resetTemperatureButton->setEnabled(false);
}

void VWindowMain::showInjectionPressure()
{
    double injectionPressure = m_pFacade->getParameters().getInjectionPressure();
    ui->injectionPressureEdit->setText(QString::number(injectionPressure));
    ui->resetInjectionPressureButton->setEnabled(false);
}

void VWindowMain::showInjectionDiameter()
{
    double injectionDiameter = m_pFacade->getParameters().getInjectionDiameter();
    ui->injectionDiameterEdit->setText(QString::number(injectionDiameter));
    ui->resetInjectionDiameterButton->setEnabled(false);
}

void VWindowMain::showInjectionPoint()
{
    m_pFacade->showInjectionPoint();
}

void VWindowMain::showVacuumPressure()
{
    double vacuumPressure = m_pFacade->getParameters().getVacuumPressure();
    ui->vacuumPressureEdit->setText(QString::number(vacuumPressure));
    ui->resetVacuumPressureButton->setEnabled(false);
}

void VWindowMain::showVacuumDiameter()
{
    double vacuumDiameter = m_pFacade->getParameters().getVacuumDiameter();
    ui->vacuumDiameterEdit->setText(QString::number(vacuumDiameter));
    ui->resetVacuumDiameterButton->setEnabled(false);
}

void VWindowMain::showTimeLimit()
{
    double timeLimit = m_pFacade->getParameters().getTimeLimit();
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(timeLimit * 1000, QTimeZone::utc());
    ui->timeEdit->setTime(dateTime.time());
    ui->resetTimeLimitButton->setEnabled(false);
}


void VWindowMain::showVacuumPoint()
{
    m_pFacade->showVacuumPoint();
}

void VWindowMain::showModelInfo()
{
    size_t nodesNumber = m_pFacade->getNodesNumber();
    if (nodesNumber > 0)
    {
        size_t trianglesNumber = m_pFacade->getTrianglesNumber();
        QVector3D modelSize;
        m_pFacade->getModelSize(modelSize);
        ui->modelInfoLabel->setText(MODEL_INFO_TEXT
                                    .arg(modelSize.x())
                                    .arg(modelSize.y())
                                    .arg(modelSize.z())
                                    .arg(nodesNumber)
                                    .arg(trianglesNumber));
        ui->modelInfoLabel->setVisible(true);
    }
    else
        ui->modelInfoLabel->setVisible(false);
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
    activateSimControls(false);
}

void VWindowMain::simulationPauseResult()
{
    ui->actionStart->setEnabled(true);
    ui->actionPause->setEnabled(false);
    ui->actionStop->setEnabled(true);
    activateSimControls(false);
}

void VWindowMain::simulationStopResult()
{
    ui->actionStart->setEnabled(true);
    ui->actionPause->setEnabled(false);
    ui->actionStop->setEnabled(false);
    activateSimControls(true);
}

void VWindowMain::activateSimControls(bool enabled)
{
    ui->layerEnableCheckBox->setEnabled(enabled);
    ui->layerColorButton->setEnabled(enabled);
    ui->selectMaterialClothButton->setEnabled(enabled);
    ui->layerRemoveButton->setEnabled(enabled);
    ui->layerCutButton->setEnabled(enabled);
    ui->addLayerButton->setEnabled(enabled);
    ui->injectionPressureEdit->setEnabled(enabled);
    ui->saveInjectionPressureButton->setEnabled(enabled);
    ui->injectionDiameterEdit->setEnabled(enabled);
    ui->injectionPlaceButton->setEnabled(enabled);
    ui->vacuumPressureEdit->setEnabled(enabled);
    ui->saveVacuumPressureButton->setEnabled(enabled);
    ui->vacuumDiameterEdit->setEnabled(enabled);
    ui->vacuumPlaceButton->setEnabled(enabled);
    ui->selectMaterialResinButton->setEnabled(enabled);
}

void VWindowMain::resetAllInputs()
{
    showTemperature();
    showInjectionPressure();
    showInjectionDiameter();
    showVacuumPressure();
    showVacuumDiameter();
}

void VWindowMain::showNewLayer()
{
    uint lastLayer = static_cast<uint>(m_pFacade->getLayersNumber()) - 1;
    VCloth::const_ptr cloth = m_pFacade->getMaterial(lastLayer);
    ui->layersListWidget->addItem(cloth->name);
    ui->layersListWidget->setCurrentRow(ui->layersListWidget->count() - 1);
}

void VWindowMain::reloadLayersList()
{
    ui->layersListWidget->clear();
    for (uint layer = 0; layer < m_pFacade->getLayersNumber(); ++layer)
    {
        VCloth::const_ptr cloth = m_pFacade->getMaterial(layer);
        ui->layersListWidget->addItem(cloth->name);
        markLayerAsEnabled(layer, m_pFacade->isLayerEnabled(layer));
        markLayerAsVisible(layer, m_pFacade->isLayerVisible(layer));
    }
    if (ui->layersListWidget->count() > 0)
        ui->layersListWidget->setCurrentRow(ui->layersListWidget->count() - 1);
}

void VWindowMain::newModel()
{
    m_pFacade->newModel();
}

void VWindowMain::loadModel()
{
    pauseSimulation();
    QSettings settings;
    QString lastDir = settings.value(QStringLiteral("import/lastDir"),
                                     QDir::homePath()).toString();
    QString fileName = QFileDialog::getOpenFileName(this, OPEN_FILE_DIALOG_TITLE, lastDir,
                                                    FILE_DIALOG_FORMATS);
    if (!fileName.isEmpty() && QFile::exists(fileName))
    {
        lastDir = QFileInfo(fileName).absolutePath();
        try
        {
            m_pFacade->loadModel(fileName);
        }
        catch (VImportException)
        {
            QMessageBox::warning(this, ERROR_TITLE, IMPORT_FROM_FILE_ERROR);
        }
        settings.setValue(QStringLiteral("import/lastDir"), lastDir);
        settings.sync();
    }
}

void VWindowMain::saveModel()
{
    pauseSimulation();
    QSettings settings;
    QString lastDir = settings.value(QStringLiteral("import/lastDir"),
                                     QDir::homePath()).toString();
    QString fileName = QFileDialog::getSaveFileName(this, SAVE_FILE_DIALOG_TITLE, lastDir,
                                                    FILE_DIALOG_FORMATS);
    if (!fileName.isEmpty())
    {
        lastDir = QFileInfo(fileName).absolutePath();
        try
        {
            m_pFacade->saveModel(fileName);
        }
        catch (VExportException)
        {
            QMessageBox::warning(this, ERROR_TITLE, EXPORT_TO_FILE_ERROR);
        }
        settings.setValue(QStringLiteral("import/lastDir"), lastDir);
        settings.sync();
    }
}

void VWindowMain::startCuttingLayer()
{
    if (ui->layersListWidget->currentIndex().isValid())
    {
        uint layer = ui->layersListWidget->currentRow();
        m_pFacade->startCuttingLayer(layer);
    }
}

void VWindowMain::cancelCuttingLayer()
{
    m_pFacade->cancelCuttingLayer();
}

void VWindowMain::askForCut()
{
    if (QMessageBox::question(this, CUT_TITLE,
                              ASK_FOR_CUT, QMessageBox::Yes|QMessageBox::No )==QMessageBox::Yes)
    {
        m_pFacade->performCut();
    }
}

void VWindowMain::askForTransform()
{
    if (QMessageBox::question(this, TRANSFORM_TITLE,
                              ASK_FOR_TRANSFORM, QMessageBox::Yes|QMessageBox::No )==QMessageBox::Yes)
    {
        m_pFacade->performTransformation();
    }
    else
    {
        m_pFacade->updateGraphicsPositions();
    }
}

void VWindowMain::updateCubeSide(int value)
{
    const int PRECISION = 3;
    int maxValue = ui->cubeSideSlider->maximum();
    float side = static_cast<float>(value) / maxValue * MAX_CUBE_SIDE;
    m_pFacade->setCubeSide(side);
    ui->cubeSideValue->setText(QString::number(side * 1000, 'g', PRECISION));
}

void VWindowMain::showCubeSide()
{
    int maxValue = ui->cubeSideSlider->maximum();
    float side = m_pFacade->getCubeSide();
    int value = static_cast<int>(side / MAX_CUBE_SIDE * maxValue + 0.5);
    if (value != ui->cubeSideSlider->value())
        ui->cubeSideSlider->setValue(value);
}

void VWindowMain::saveSizes()
{
    QSettings settings;
    settings.setValue("window/geometry", saveGeometry());
    settings.setValue("window/windowState", saveState());
    settings.setValue("window/splitter", ui->splitter->saveGeometry());
    settings.setValue("window/splitterState", ui->splitter->saveState());
    settings.sync();
}

void VWindowMain::loadSizes()
{
    QSettings settings;
    restoreGeometry(settings.value("window/geometry").toByteArray());
    restoreState(settings.value("window/windowState").toByteArray());
    ui->splitter->restoreGeometry(settings.value("window/splitter").toByteArray());
    ui->splitter->restoreState(settings.value("window/splitterState").toByteArray());
}

/**
  * Slideshow methods
  */

void VWindowMain::setSlideshowDir(const QString &dir)
{
    m_pSlideshowShooter->setDirName(dir);
}

void VWindowMain::setSlideshowPeriod(float period)
{
    m_pSlideshowShooter->setPeriod(period);
}

void VWindowMain::setVideoFile(const QString &file)
{
    m_videoFile = file;
}

void VWindowMain::setVideoPeriod(float period)
{
    m_pVideoShooter->setPeriod(period);
}

const QString & VWindowMain::getSlideshowDir() const
{
    return m_pSlideshowShooter->getDirName();
}

float VWindowMain::getSlideshowPeriod() const
{
    return m_pSlideshowShooter->getPeriod();
}

void VWindowMain::startSlideshow()
{
    m_pSlideshowShooter->start();
}

void VWindowMain::stopSlideshow()
{
    m_pSlideshowShooter->stop();
}

const QString & VWindowMain::getVideoFile() const
{
    //TODO
    return m_videoFile;
}

float VWindowMain::getVideoPeriod() const
{
    //TODO
    return m_pVideoShooter->getPeriod();
}

void VWindowMain::startVideo()
{
    //TODO
    QDir videoDir(VIDEO_DIR_PATH);
    if (videoDir.exists())
    {
        bool result = videoDir.removeRecursively();
        if (!result)
            return;
    }
    m_pVideoShooter->start();
}

void VWindowMain::stopVideo()
{
    m_pVideoShooter->stop();
}

void VWindowMain::saveVideo()
{
    //TODO
    //TODO clear temporary folder
}

void VWindowMain::showSlideshowPeriod()
{
    float period = m_pSlideshowShooter->getPeriod();
    ui->slideshowPeriodSpinBox->setValue(period);
    ui->resetSlideshowPeriodButton->setEnabled(false);
}

void VWindowMain::showVideoFrequency()
{

}

void VWindowMain::loadShootersSettings()
{
    QSettings settings;
    QString slideShowDir, videoDir;
    float slideshowPeriod;
    int videoFrequency;

    slideShowDir = settings.value(QStringLiteral("slideshow/directory"), m_pSlideshowShooter->getDirName()).toString();
    slideshowPeriod = settings.value(QStringLiteral("slideshow/period"), m_pSlideshowShooter->getPeriod()).toFloat();

    videoDir = settings.value(QStringLiteral("video/directory"), m_pVideoShooter->getDirName()).toString();
    //videoFrequency= settings.value(QStringLiteral("video/frequency"), m_pVideoShooter->getFrequency()).toInt(); //TODO
    m_pSlideshowShooter->setDirName(slideShowDir);
    m_pSlideshowShooter->setPeriod(slideshowPeriod);
    m_pVideoShooter->setDirName(videoDir);
    //m_pVideoShooter->setFrequency(videoFrequency); //TODO
}

void VWindowMain::saveSootersSettings()
{
    QSettings settings;
    settings.setValue(QStringLiteral("slideshow/directory"), m_pSlideshowShooter->getDirName());
    settings.setValue(QStringLiteral("slideshow/period"), m_pSlideshowShooter->getPeriod());

    settings.setValue(QStringLiteral("video/directory"), m_pVideoShooter->getDirName());
    //settings.setValue(QStringLiteral("video/frequency"), m_pVideoShooter->getFrequency()); //TODO
    settings.sync();
}

void VWindowMain::enableButtonsShootingWindows(bool enable)
{
    ui->chooseSlideshowDirButton->setEnabled(enable);
    ui->chooseVideoDirButton->setEnabled(enable);
    ui->actionOpen->setEnabled(enable);
    ui->actionSave->setEnabled(enable);
}

/**
 * Slots
 */

void VWindowMain::m_on_layer_creation_from_file_available(const VCloth& material, const QString& filename,
                                                          VLayerAbstractBuilder::VUnit units)
{
    addLayerFromFile(material, filename, units);
}

void VWindowMain::m_on_layer_creation_manual_available(const VCloth& material,const QPolygonF& polygon,
                                                       double characteristicLength)
{
    addLayerFromPolygon(material, polygon, characteristicLength);
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

void VWindowMain::m_on_layers_cleared()
{
    reloadLayersList();
    showModelInfo();
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
    if (QMessageBox::question(this, REMOVE_TITLE,
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

void VWindowMain::m_on_layer_removed(uint layer)
{
    removeLayerFromList(layer);
}

void VWindowMain::m_on_material_changed(uint layer)
{
    updateLayerMaterialInfo(layer);
}

void VWindowMain::m_on_layer_enabled(uint layer, bool enable)
{
    markLayerAsEnabled(layer, enable);
}

void VWindowMain::m_on_layer_added()
{
    showNewLayer();
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

void VWindowMain::m_on_resin_changed()
{
    updateResinInfo();
}

void VWindowMain::m_on_injection_diameter_set(double)
{
    showInjectionDiameter();
}

void VWindowMain::m_on_vacuum_diameter_set(double)
{
    showVacuumDiameter();
}

void VWindowMain::m_on_temperature_set(double)
{
    showTemperature();
}

void VWindowMain::m_on_vacuum_pressure_set(double)
{
    showVacuumPressure();
}

void VWindowMain::m_on_injection_pressure_set(double)
{
    showInjectionPressure();
}

void VWindowMain::m_on_time_limit_set(double)
{
    showTimeLimit();
}

void VWindowMain::m_on_time_limit_mode_switched(bool on)
{
    ui->timeLimitCheckBox->setChecked(on);
}

void VWindowMain::m_on_canceled_waiting_for_injection_point()
{
    ui->injectionPlaceButton->setChecked(false);
}

void VWindowMain::m_on_canceled_waiing_for_vacuum_point()
{
    ui->vacuumPlaceButton->setChecked(false);
}

void VWindowMain::m_on_layer_visibility_changed(uint layer, bool visible)
{
    markLayerAsVisible(layer, visible);
}

void VWindowMain::m_on_model_loaded()
{
    reloadLayersList();
}

void VWindowMain::m_on_selection_made()
{
    askForCut();
}

void VWindowMain::m_on_got_transformation()
{
    askForTransform();
}

void VWindowMain::m_on_model_config_updated()
{
    showModelInfo();
}

void VWindowMain::m_on_selection_enabled(bool checked)
{
    ui->layerCutButton->setChecked(checked);
}

void VWindowMain::m_on_cube_side_changed(float)
{
    showCubeSide();
}

void VWindowMain::m_on_slideshow_started()
{
    enableButtonsShootingWindows(false);
    ui->actionSlideshow->setChecked(true);
    setWindowFlags(windowFlags() | ON_TOP_FLAGS);
    show();
}

void VWindowMain::m_on_slideshow_stopped()
{
    enableButtonsShootingWindows(true);
    ui->actionSlideshow->setChecked(false);
    setWindowFlags(windowFlags() ^ ON_TOP_FLAGS);
    show();
}

void VWindowMain::m_on_slideshow_directory_changed()
{
    const QString& dir = m_pSlideshowShooter->getDirName();
    ui->slideshowDirLabel->setText(dir);
}

void VWindowMain::m_on_slideshow_period_changed()
{
    showSlideshowPeriod();
}

void VWindowMain::m_on_video_started()
{
    enableButtonsShootingWindows(false);
    ui->actionVideo->setChecked(true);
    setWindowFlags(windowFlags() | ON_TOP_FLAGS);
    show();
}

void VWindowMain::m_on_video_stopped()
{
    enableButtonsShootingWindows(true);
    ui->actionVideo->setChecked(false);
    setWindowFlags(windowFlags() ^ ON_TOP_FLAGS);
    show();
    saveVideo();
}

void VWindowMain::m_on_video_directory_changed()
{
    const QString& dir = m_pVideoShooter->getDirName();
    ui->videoDirLabel->setText(dir);
}

void VWindowMain::m_on_video_period_changed()
{
 //TODO
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

void VWindowMain::on_resetInjectionDiameterButton_clicked()
{
    showInjectionDiameter();
    cancelInjectionPointSelection();
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
    cancelVacuumPointSelection();
}

void VWindowMain::on_showVacuumPlaceButton_clicked()
{
    showVacuumPoint();
}

void VWindowMain::on_actionPause_triggered()
{
    pauseSimulation();
}

void VWindowMain::on_layerColorButton_clicked()
{
    selectColor();
}

void VWindowMain::on_temperatureEdit_textEdited(const QString &)
{
    ui->resetTemperatureButton->setEnabled(true);
}

void VWindowMain::on_injectionPressureEdit_textEdited(const QString &)
{
    ui->resetInjectionPressureButton->setEnabled(true);
}

void VWindowMain::on_injectionDiameterEdit_textEdited(const QString &)
{
    ui->resetInjectionDiameterButton->setEnabled(true);
}

void VWindowMain::on_vacuumPressureEdit_textEdited(const QString &)
{
    ui->resetVacuumPressureButton->setEnabled(true);
}

void VWindowMain::on_vacuumDiameterEdit_textEdited(const QString &)
{
    ui->resetVacuumDiameterButton->setEnabled(true);
}

void VWindowMain::on_timeEdit_timeChanged(const QTime&)
{
    ui->resetTimeLimitButton->setEnabled(true);
}

void VWindowMain::on_actionNew_triggered()
{
    newModel();
}

void VWindowMain::on_actionOpen_triggered()
{
    loadModel();
}

void VWindowMain::on_actionSave_triggered()
{
    saveModel();
}

void VWindowMain::on_saveTimeLimitButton_clicked()
{
    saveTimeLimit();
}

void VWindowMain::on_resetTimeLimitButton_clicked()
{
    showTimeLimit();
}

void VWindowMain::on_timeLimitCheckBox_clicked(bool checked)
{
    enableTimeLimitMode(checked);
}

void VWindowMain::on_layerCutButton_clicked(bool checked)
{
    if (checked)
        startCuttingLayer();
    else
        cancelCuttingLayer();
}

void VWindowMain::on_cubeSideSlider_valueChanged(int value)
{
    updateCubeSide(value);
}


void VWindowMain::on_actionSlideshow_triggered(bool checked)
{
    if(checked)
        startSlideshow();
    else
        stopSlideshow();
}

void VWindowMain::on_actionVideo_triggered(bool checked)
{

}

void VWindowMain::on_chooseSlideshowDirButton_clicked()
{
    m_pFacade->pauseSimulation();
    QString lastDir = m_pSlideshowShooter->getDirName();
    QString dirName = QFileDialog::getExistingDirectory(this, SLIDESHOW_DIR_DIALOG_TITLE, lastDir);
    QDir parentDir(dirName);
    if (!dirName.isEmpty() && parentDir.exists())
    {
        m_pSlideshowShooter->setDirName(dirName);
    }
}

void VWindowMain::on_slideshowPeriodSpinBox_valueChanged(double arg1)
{
    ui->resetSlideshowPeriodButton->setEnabled(true);
}

void VWindowMain::on_resetSlideshowPeriodButton_clicked()
{
    showSlideshowPeriod();
}

void VWindowMain::on_saveSlideshowPeriodButton_clicked()
{
    float value = static_cast<float>(ui->slideshowPeriodSpinBox->value());
    m_pSlideshowShooter->setPeriod(value);
}
