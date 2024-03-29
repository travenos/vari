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
#include <QCloseEvent>
#include <QDesktopServices>

#include "VWindowMain.h"
#include "ui_VWindowMain.h"
#include "VWindowCloth.h"
#include "VWindowResin.h"
#include "VWindowLayer.h"
#include "VWindowPolygon.h"
#include "VScreenShooter.h"
#include "VVideoShooter.h"
#include "VImageTextWriters.h"
#include "sim/VSimulationFacade.h"
#include "sim/structures/VExceptions.h"
#include "sim/structures/VTable.h"

const QString VWindowMain::ERROR_TITLE("Ошибка");
const QString VWindowMain::INFO_TITLE("Информация");
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
const QString VWindowMain::EXIT_TITLE("Выйти?");
const QString VWindowMain::ASK_FOR_EXIT_TEXT("Имеются несохранённые параметры. Вы уверены, что хотите выйти?");
const QString VWindowMain::CLOTH_INFO_TEXT("<html><head/><body>"
                                           "Материал: &quot;%1&quot;<br>"
                                           "Толщина: %2 м<br>"
                                           "Продольная проницаемость: %3 м<span style=\" vertical-align:super;\">2</span><br>"
                                           "Поперечная проницаемость: %4 м<span style=\" vertical-align:super;\">2</span><br>"
                                           "Пористость: %5"
                                           "</body></html>");
const QString VWindowMain::RESIN_INFO_TEXT("<html><head/><body>"
                                           "Материал: &quot;%1&quot;<br>"
                                           "Вязкость при 25 °C: %2 Па·с<br>"
                                           "Энергия активации вязкого течения: %3 Дж/моль<br>"
                                           "Время жизни при 25 °C: %4 с<br>"
                                           "Энергия активации процесса отвеждения: %5 Дж/моль<br>"
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
const QString VWindowMain::VIDEO_DIR_DIALOG_TITLE("Путь сохранения видео");
const QString VWindowMain::SAVING_VIDEO_ERROR("Ошибка записи видео");
const QString VWindowMain::SAVING_VIDEO_INFO("Видео сохранено в файл %1");
const Qt::WindowFlags VWindowMain::ON_TOP_FLAGS = (Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint | Qt::Dialog | Qt::WindowTitleHint);

const QStringList VWindowMain::LAYERS_TABLE_LABELS({QString("Имя"),
                                                    QString("Материал"),
                                                    QString("Угол"),
                                                    QString("Цвет")});

VWindowMain::VWindowMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VWindowMain),
    m_pWindowLayer(nullptr),
    m_pWindowCloth(nullptr),
    m_pWindowResin(nullptr),
    m_pWindowPolygon(nullptr),
    m_pTemperatureValidator(new QDoubleValidator),
    m_pPressureValidator(new QDoubleValidator),
    m_pDiameterValidator(new QDoubleValidator),
    m_pAngleValidator(new QDoubleValidator),
    m_pSlideshowShooter(new VScreenShooter),
    m_pVideoShooter(new VVideoShooter),
    m_isSaved(false)
{
    ui->setupUi(this);
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 0);
    ui->simParamSplitter->setStretchFactor(0, 0);
    ui->simParamSplitter->setStretchFactor(1, 1);
    ui->layerParamBox->setVisible(false);
    ui->modelInfoLabel->setVisible(false);
    setContextMenuPolicy(Qt::NoContextMenu);
    m_pFacade.reset(new VSimulationFacade(ui->viewerWidget));
    connectSimulationSignals();
    setupValidators();
    setupSpinboxesLocales();
    m_pFacade->loadSavedParameters();
    showCubeSide();
    loadWindowSettings();
    m_pImgTextWriter.reset(new VSimInfoImageTextWriter(m_pFacade));
    m_pSlideshowShooter->setWidget(m_pFacade->getGLWidget());
    m_pSlideshowShooter->setImageTextWriter(m_pImgTextWriter);
    configureVideoShooter();
    QDir(VVideoShooter::SLIDES_DIR_PATH).removeRecursively();
    loadShootersSettings();

    ui->layersTableWidget->setColumnCount(LAYERS_TABLE_LABELS.size());
    ui->layersTableWidget->setHorizontalHeaderLabels(LAYERS_TABLE_LABELS);
    ui->layersTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->layersTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->layersTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->layersTableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);

    this->setWindowTitle(QCoreApplication::applicationName());
}

void VWindowMain::connectSimulationSignals()
{
    connect(m_pFacade.get(), SIGNAL(layerRemoved(uint)),
            this, SLOT(m_on_layer_removed(uint)));
    connect(m_pFacade.get(), SIGNAL(materialChanged(uint)),
            this, SLOT(m_on_material_changed(uint)));
    connect(m_pFacade.get(), SIGNAL(layerNameChanged(uint)),
            this, SLOT(m_on_layer_name_changed(uint)));
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
    connect(m_pFacade.get(), SIGNAL(injectionDiameterSet(float)),
            this, SLOT(m_on_injection_diameter_set(float)));
    connect(m_pFacade.get(), SIGNAL(vacuumDiameterSet(float)),
            this, SLOT(m_on_vacuum_diameter_set(float)));
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
    connect(m_pFacade.get(), SIGNAL(lifetimeConsiderationSwitched(bool)),
            this, SLOT(m_on_lifetime_consideration_switched(bool)));
    connect(m_pFacade.get(), SIGNAL(canceledWaitingForInjectionPoint()),
            this, SLOT(m_on_canceled_waiting_for_injection_point()));
    connect(m_pFacade.get(), SIGNAL(canceledWaitingForVacuumPoint()),
            this, SLOT(m_on_canceled_waiing_for_vacuum_point()));
    connect(m_pFacade.get(), SIGNAL(layersCleared()),
            this, SLOT(m_on_layers_cleared()));
    connect(m_pFacade.get(), SIGNAL(modelLoaded()),
            this, SLOT(m_on_model_loaded()));
    connect(m_pFacade.get(), SIGNAL(modelSaved()),
            this, SLOT(m_on_model_saved()));
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
    connect(m_pFacade.get(), SIGNAL(filenameChanged(const QString &)),
            this, SLOT(m_on_filename_changed(const QString &)));
    connect(m_pFacade.get(), SIGNAL(layersSwapped(uint, uint)),
            this, SLOT(m_on_layers_swapped(uint, uint)));

    connect(m_pFacade.get(), SIGNAL(tableSizeSet(float, float)),
            this, SLOT(m_on_table_size_set(float, float)));
    connect(m_pFacade.get(), SIGNAL(tableInjectionCoordsSet(float, float)),
            this, SLOT(m_on_table_injection_coords_set(float, float)));
    connect(m_pFacade.get(), SIGNAL(tableVacuumCoordsSet(float, float)),
            this, SLOT(m_on_table_vacuum_coords_set(float, float)));
    connect(m_pFacade.get(), SIGNAL(tableInjectionDiameterSet(float)),
            this, SLOT(m_on_table_injection_diameter_set(float)));
    connect(m_pFacade.get(), SIGNAL(tableVacuumDiameterSet(float)),
            this, SLOT(m_on_table_vacuum_diameter_set(float)));
    connect(m_pFacade.get(), SIGNAL(useTableParametersSet(bool)),
            this, SLOT(m_on_use_table_parameters_set(bool)));

    connect(m_pFacade.get(), SIGNAL(stopOnVacuumFullSwitched(bool)),
            this, SLOT(m_stop_on_vacuum_full_switched(bool)));
    connect(m_pFacade.get(), SIGNAL(additionalControlsEnabled(bool)),
            this, SLOT(m_on_additional_controls_enabled(bool)));

    connect(m_pFacade.get(), SIGNAL(layerRebuilt(uint)),
            this, SLOT(m_on_layer_rebuilt(uint)));

    connect(m_pSlideshowShooter.get(), SIGNAL(processStarted()), this, SLOT(m_on_slideshow_started()));
    connect(m_pSlideshowShooter.get(), SIGNAL(processStopped()), this, SLOT(m_on_slideshow_stopped()));
    connect(m_pSlideshowShooter.get(), SIGNAL(directoryChanged()), this, SLOT(m_on_slideshow_directory_changed()));
    connect(m_pSlideshowShooter.get(), SIGNAL(periodChanged()), this, SLOT(m_on_slideshow_period_changed()));
    connect(m_pSlideshowShooter.get(), SIGNAL(suffixDirNameChanged()), this, SLOT(m_on_slideshow_suffix_dirname_changed()));
}

void VWindowMain::setupValidators()
{
    m_pTemperatureValidator->setLocale(QLocale::C);
    m_pPressureValidator->setLocale(QLocale::C);
    m_pDiameterValidator->setLocale(QLocale::C);
    m_pAngleValidator->setLocale(QLocale::C);
    m_pTemperatureValidator->setBottom(-VSimulationParameters::KELVINS_IN_0_CELSIUS);
    m_pPressureValidator->setBottom(0);
    m_pDiameterValidator->setBottom(0);
    ui->temperatureEdit->setValidator(m_pTemperatureValidator);
    ui->injectionPressureEdit->setValidator(m_pPressureValidator);
    ui->vacuumPressureEdit->setValidator(m_pPressureValidator);
    ui->injectionDiameterEdit->setValidator(m_pDiameterValidator);
    ui->vacuumDiameterEdit->setValidator(m_pDiameterValidator);
    ui->layerAngleEdit->setValidator(m_pAngleValidator);
}

void VWindowMain::setupSpinboxesLocales()
{
    ui->tableXSpinBox->setLocale(QLocale::C);
    ui->tableYSpinBox->setLocale(QLocale::C);
    ui->tableInjectionXSpinBox->setLocale(QLocale::C);
    ui->tableInjectionYSpinBox->setLocale(QLocale::C);
    ui->tableInjectionDiameterSpinBox->setLocale(QLocale::C);
    ui->tableVacuumXSpinBox->setLocale(QLocale::C);
    ui->tableVacuumYSpinBox->setLocale(QLocale::C);
    ui->tableVacuumDiameterSpinBox->setLocale(QLocale::C);
    ui->slideshowPeriodSpinBox->setLocale(QLocale::C);
    ui->videoFrequencySpinBox->setLocale(QLocale::C);
}

void VWindowMain::configureVideoShooter()
{
    m_pVideoShooter->setWidget(m_pFacade->getGLWidget());
    m_pVideoShooter->setImageTextWriter(m_pImgTextWriter);

    connect(m_pVideoShooter.get(), SIGNAL(processStarted()), this, SLOT(m_on_video_started()));
    connect(m_pVideoShooter.get(), SIGNAL(processStopped()), this, SLOT(m_on_video_stopped()));
    connect(m_pVideoShooter.get(), SIGNAL(directoryChanged()), this, SLOT(m_on_video_directory_changed()));
    connect(m_pVideoShooter.get(), SIGNAL(frequencyChanged()), this, SLOT(m_on_video_frequency_changed()));
    connect(m_pVideoShooter.get(), SIGNAL(suffixFileNameChanged()), this, SLOT(m_on_video_suffix_filename_changed()));
    connect(m_pVideoShooter.get(), SIGNAL(videoSavingStarted()), this, SLOT(m_on_video_saving_started()));
    connect(m_pVideoShooter.get(), SIGNAL(videoSavingFinished(bool)), this, SLOT(m_on_video_saving_finished(bool)));
}

void VWindowMain::addNewVideoShooter()
{
    std::shared_ptr<VVideoShooter> videoShooter(new VVideoShooter);

    videoShooter->setDirPath(m_pVideoShooter->getDirPath());
    videoShooter->setFrequency(m_pVideoShooter->getFrequency());
    videoShooter->setSuffixFileName(m_pVideoShooter->getSuffixFileName());
    m_oldVideoShootersList.push_back(m_pVideoShooter);

    disconnect(m_pVideoShooter.get(), SIGNAL(processStarted()), this, SLOT(m_on_video_started()));
    disconnect(m_pVideoShooter.get(), SIGNAL(processStopped()), this, SLOT(m_on_video_stopped()));
    disconnect(m_pVideoShooter.get(), SIGNAL(directoryChanged()), this, SLOT(m_on_video_directory_changed()));
    disconnect(m_pVideoShooter.get(), SIGNAL(frequencyChanged()), this, SLOT(m_on_video_frequency_changed()));
    disconnect(m_pVideoShooter.get(), SIGNAL(suffixFileNameChanged()), this, SLOT(m_on_video_suffix_filename_changed()));
    disconnect(m_pVideoShooter.get(), SIGNAL(videoSavingStarted()), this, SLOT(m_on_video_saving_started()));
    disconnect(m_pVideoShooter.get(), SIGNAL(videoSavingFinished(bool)), this, SLOT(m_on_video_saving_finished(bool)));

    m_pVideoShooter = videoShooter;
    configureVideoShooter();
}

void VWindowMain::clearFinishedVideoShooters()
{
    auto it = m_oldVideoShootersList.begin();
    while (it != m_oldVideoShootersList.end())
    {
        if (!(*it)->isSaving() && !(*it)->isWorking())
        {
            m_oldVideoShootersList.erase(it++);
        }
        else
            ++it;
    }
}

VWindowMain::~VWindowMain()
{
    disconnect(m_pSlideshowShooter.get(), SIGNAL(processStarted()), this, SLOT(m_on_slideshow_started()));
    disconnect(m_pSlideshowShooter.get(), SIGNAL(processStopped()), this, SLOT(m_on_slideshow_stopped()));
    disconnect(m_pVideoShooter.get(), SIGNAL(processStarted()), this, SLOT(m_on_video_started()));
    disconnect(m_pVideoShooter.get(), SIGNAL(processStopped()), this, SLOT(m_on_video_stopped()));
    disconnect(m_pVideoShooter.get(), SIGNAL(videoSavingStarted()), this, SLOT(m_on_video_saving_started()));
    disconnect(m_pVideoShooter.get(), SIGNAL(videoSavingFinished(bool)), this, SLOT(m_on_video_saving_finished(bool)));
    stopVideo();
    stopSlideshow();
    saveSootersSettings();
    m_pSlideshowShooter.reset();
    m_pVideoShooter.reset();
    m_pFacade->saveParameters();
    m_pFacade.reset();
    delete ui;
    deleteWindowLayer();
    deleteWindowResin();
    deleteWindowCloth();
    deleteWindowPolygon();
    VSqlDatabase::deleteInstance();
    delete m_pPressureValidator;
    delete m_pTemperatureValidator;
    delete m_pDiameterValidator;
    delete m_pAngleValidator;
    #ifdef DEBUG_MODE
        qInfo() << "VWindowMain destroyed";
    #endif
}

void VWindowMain::closeEvent(QCloseEvent * event)
{
    if (!m_isSaved && QMessageBox::question(this, EXIT_TITLE,
                      ASK_FOR_EXIT_TEXT, QMessageBox::Yes|QMessageBox::No )!=QMessageBox::Yes)
    {
        event->ignore();
    }
    else
    {
        saveWindowSettings();
    }
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

void VWindowMain::deleteWindowPolygon()
{
    if (m_pWindowPolygon != nullptr)
    {
        m_pWindowPolygon->deleteLater();
        m_pWindowPolygon = nullptr;
    }
}

void VWindowMain::addLayerFromFile(const VCloth& material, const QString& filename,
                                   const QString& layerName,
                                   VLayerAbstractBuilder::VUnit units)
{
    try
    {
        m_pFacade->newLayerFromFile(material, filename, layerName, units);
    }
    catch (VImportException &)
    {
        QMessageBox::warning(this, ERROR_TITLE, IMPORT_FROM_FILE_ERROR);
    }
    catch (VSimulatorException &)
    {
        QMessageBox::warning(this, ERROR_TITLE, IMPORT_WHEN_SIMULATING_ERROR);
    }
}

void VWindowMain::addLayerFromPolygon(const VCloth& material, const QPolygonF& polygon,
                                      double characteristicLength, const QString &layerName)
{
    try
    {
        m_pFacade->newLayerFromPolygon(material, polygon, characteristicLength, layerName);
    }
    catch (VImportException &)
    {
        QMessageBox::warning(this, ERROR_TITLE, IMPORT_MANUAL_ERROR);
    }
    catch (VSimulatorException &)
    {
        QMessageBox::warning(this, ERROR_TITLE, IMPORT_WHEN_SIMULATING_ERROR);
    }
}

void VWindowMain::showWindowLayer()
{
    if (m_pWindowLayer == nullptr)
    {
        m_pWindowLayer = new VWindowLayer(this, m_pFacade);
        connect(m_pWindowLayer,
                SIGNAL(creationFromFileAvailable(const VCloth&, const QString&, const QString&, VLayerAbstractBuilder::VUnit)),
                this,
                SLOT(m_on_layer_creation_from_file_available(const VCloth&, const QString&, const QString&, VLayerAbstractBuilder::VUnit)));
        connect(m_pWindowLayer,
                SIGNAL(creationManualAvailable(const VCloth&,const QPolygonF&, double, const QString&)),
                this,
                SLOT(m_on_layer_creation_manual_available(const VCloth&,const QPolygonF&, double, const QString&)));
        connect(m_pWindowLayer,SIGNAL(windowClosed()), this, SLOT(m_on_layer_window_closed()));
    }
    m_pWindowLayer->show();
    m_pWindowLayer->activateWindow();
}

void VWindowMain::selectLayer()
{
    if (ui->layersTableWidget->currentIndex().isValid()
            && ui->layersTableWidget->currentRow() < int(m_pFacade->getLayersNumber()))
    {
        uint layer = ui->layersTableWidget->currentRow();
        VCloth::const_ptr cloth = m_pFacade->getMaterial(layer);
        bool visible = m_pFacade->isLayerVisible(layer);
        bool enabled = m_pFacade->isLayerEnabled(layer);
        ui->layerParamBox->setVisible(true);
        ui->layerVisibleCheckBox->setChecked(visible);
        ui->layerEnableCheckBox->setChecked(enabled);
        ui->layerInfoLabel->setText(CLOTH_INFO_TEXT.arg(cloth->getName()).arg(cloth->getCavityHeight())
                                             .arg(cloth->getXPermeability()).arg(cloth->getYPermeability()).arg(cloth->getPorosity()));
        showColor(cloth->getBaseColor());
        ui->layerAngleEdit->setText(QString::number(cloth->getAngleDeg()));
        ui->resetLayerAngleButton->setEnabled(false);
        ui->layerUpButton->setEnabled(layer > 0 && m_pFacade->isSimulationStopped());
        ui->layerDownButton->setEnabled(layer < m_pFacade->getLayersNumber() - 1u && m_pFacade->isSimulationStopped());
        ui->layerNameEdit->setText(m_pFacade->getLayerName(layer));
        ui->resetLayerNameButton->setEnabled(false);
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
    if (ui->layersTableWidget->currentIndex().isValid())
    {
        int layer = ui->layersTableWidget->currentRow();
        if (layer >= 0)
            m_pFacade->enableLayer(static_cast<uint>(layer), enable);
    }
}

void VWindowMain::setVisibleLayer(bool visible)
{
    if (ui->layersTableWidget->currentIndex().isValid())
    {
        int layer = ui->layersTableWidget->currentRow();
        if (layer >= 0)
            m_pFacade->setVisible(static_cast<uint>(layer), visible);
    }
}

void VWindowMain::enableTimeLimitMode(bool checked)
{
    m_pFacade->setTimeLimitMode(checked);
}

void VWindowMain::removeLayer()
{
    if (ui->layersTableWidget->currentIndex().isValid())
    {
        int layer = ui->layersTableWidget->currentRow();
        m_pFacade->removeLayer(layer);
    }
}

void VWindowMain::showWindowCloth()
{
    if (m_pWindowCloth == nullptr)
    {
        m_pWindowCloth = new VWindowCloth(this);
        connect(m_pWindowCloth, SIGNAL(gotMaterial(const QString &, float, float, float, float)),
                this, SLOT(m_on_got_cloth(const QString &, float, float, float, float)));
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
        connect(m_pWindowResin, SIGNAL(gotMaterial(const QString &, float, float, float, float)),
                this, SLOT(m_on_got_resin(const QString &, float, float, float, float)));
        connect(m_pWindowResin,SIGNAL(windowClosed()), this, SLOT(m_on_resin_window_closed()));
    }
    m_pWindowResin->show();
    m_pWindowResin->activateWindow();
}

void VWindowMain::setCloth(const QString & name, float cavityheight, float xpermeability, float ypermeability, float porosity)
{
    if (ui->layersTableWidget->currentIndex().isValid())
    {
        int layer = ui->layersTableWidget->currentRow();
        VCloth cloth = *(m_pFacade->getMaterial(layer));
        cloth.setCavityHeight(cavityheight);
        cloth.setXPermeability(xpermeability);
        cloth.setYPermeability(ypermeability);
        cloth.setPorosity(porosity);
        cloth.setName(name);
        m_pFacade->setMaterial(layer, cloth);
    }
}

void VWindowMain::selectColor()
{
    if (ui->layersTableWidget->currentIndex().isValid())
    {
        int layer = ui->layersTableWidget->currentRow();
        VCloth cloth = *(m_pFacade->getMaterial(layer));
        QColor color = QColorDialog::getColor(cloth.getBaseColor(), this);

        if (color.isValid())
        {
            cloth.setBaseColor(color);
            m_pFacade->setMaterial(layer, cloth);
        }
    }
}

void VWindowMain::setResin(const QString & name , float viscosity, float viscTempcoef,
                           float lifetime, float lifetimeTempcoef)
{
    VResin resin;
    resin.defaultViscosity = viscosity;
    resin.defaultLifetime = lifetime;
    resin.viscTempcoef = viscTempcoef;
    resin.lifetimeTempcoef = lifetimeTempcoef;
    resin.name = name;
    m_pFacade->setResin(resin);
}

void VWindowMain::removeLayerFromList(int layer)
{
    if(layer < ui->layersTableWidget->rowCount())
    {
        bool reselectFlag = false;
        if (layer == ui->layersTableWidget->currentRow())
        {
            if(layer > 0)
                ui->layersTableWidget->selectRow(layer - 1);
            else
                reselectFlag = true;
        }
        ui->layersTableWidget->removeRow(layer);
        if (reselectFlag)
            selectLayer();
    }
}

void VWindowMain::updateLayerMaterialInfo(int layer)
{
    VCloth::const_ptr cloth = m_pFacade->getMaterial(layer);
    ui->layersTableWidget->item(layer, 1)->setText(cloth->getName());
    ui->layersTableWidget->item(layer, 2)->setText(QString::number(cloth->getAngleDeg()));
    ui->layersTableWidget->item(layer, 3)->setBackgroundColor(cloth->getBaseColor());
    if ( layer == ui->layersTableWidget->currentRow())
    {
        ui->layerInfoLabel->setText(CLOTH_INFO_TEXT.arg(cloth->getName())
                                    .arg(cloth->getCavityHeight())
                                    .arg(cloth->getXPermeability())
                                    .arg(cloth->getYPermeability())
                                    .arg(cloth->getPorosity()));
        showColor(cloth->getBaseColor());
        ui->layerAngleEdit->setText(QString::number(cloth->getAngleDeg()));
        ui->resetLayerAngleButton->setEnabled(false);
    }
}

void VWindowMain::updateLayerName(int layer)
{
    const QString & layerName = m_pFacade->getLayerName(layer);
    ui->layersTableWidget->item(layer, 0)->setText(layerName);
    if ( layer == ui->layersTableWidget->currentRow())
    {
        ui->layerNameEdit->setText(layerName);
        ui->resetLayerNameButton->setEnabled(false);
    }
}


void VWindowMain::showAngle()
{
    if (ui->layersTableWidget->currentIndex().isValid())
    {
        int layer = ui->layersTableWidget->currentRow();
        if (layer >= 0 && layer < int(m_pFacade->getLayersNumber()))
        {
            VCloth::const_ptr cloth = m_pFacade->getMaterial(layer);
            ui->layerAngleEdit->setText(QString::number(cloth->getAngleDeg()));
            ui->resetLayerAngleButton->setEnabled(false);
        }
    }
}

void VWindowMain::saveAngle()
{
    if (ui->layersTableWidget->currentIndex().isValid())
    {
        int layer = ui->layersTableWidget->currentRow();
        if (layer >= 0 && layer < int(m_pFacade->getLayersNumber()))
        {
            bool result;
            double angle = ui->layerAngleEdit->text().toDouble(&result);
            if (result)
            {
                VCloth cloth = *(m_pFacade->getMaterial(layer));
                cloth.setAngleDeg(angle);
                m_pFacade->setMaterial(layer, cloth);
            }

        }
    }
}

void VWindowMain::updateResinInfo()
{
    VResin resin = m_pFacade->getParameters().getResin();
    ui->resinInfoLabel->setText(RESIN_INFO_TEXT.arg(resin.name).arg(resin.defaultViscosity)
                                .arg(resin.viscTempcoef).arg(resin.defaultLifetime)
                                .arg(resin.lifetimeTempcoef));
}

void VWindowMain::markLayerAsEnabled(int layer, bool enable)
{
    if(layer < ui->layersTableWidget->rowCount())
    {
        QColor textColor;
        if (!m_pFacade->isLayerEnabled(layer))
            textColor = INACTIVE_COLOR;
        else if (!m_pFacade->isLayerVisible(layer))
            textColor = INVISIBLE_COLOR;
        else
            textColor = ACTIVE_COLOR;
        for (int i{0}; i < ui->layersTableWidget->columnCount(); ++i)
            ui->layersTableWidget->item(layer, i)->setTextColor(textColor);
        if (ui->layersTableWidget->currentRow() == layer)
            ui->layerEnableCheckBox->setChecked(enable);
    }
}

void VWindowMain::markLayerAsVisible(int layer, bool visible)
{
    if(layer < ui->layersTableWidget->rowCount())
    {
        QColor textColor;
        if (!m_pFacade->isLayerEnabled(layer))
            textColor = INACTIVE_COLOR;
        else if (!m_pFacade->isLayerVisible(layer))
            textColor = INVISIBLE_COLOR;
        else
            textColor = ACTIVE_COLOR;
        for (int i{0}; i < ui->layersTableWidget->columnCount(); ++i)
            ui->layersTableWidget->item(layer, i)->setTextColor(textColor);
        if (ui->layersTableWidget->currentRow() == layer)
            ui->layerVisibleCheckBox->setChecked(visible);
    }
}

void VWindowMain::injectionPointSelectionResult()
{
    ui->injectionPlaceButton->setChecked(false);
}

void VWindowMain::startInjectionPointSelection()
{
    float diameter;
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
    float diameter;
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
    float injectionDiameter = m_pFacade->getParameters().getInjectionDiameter();
    ui->injectionDiameterEdit->setText(QString::number(injectionDiameter));
    ui->resetInjectionDiameterButton->setEnabled(false);
}

void VWindowMain::showInjectionPoint()
{
    m_pFacade->showInjectionPoint();
}

void VWindowMain::showVacuumPressure()
{
    float vacuumPressure = m_pFacade->getParameters().getVacuumPressure();
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

bool VWindowMain::readNumber(const QLineEdit * lineEdit, float &output) const
{
    bool ok;
    QString numberStr = lineEdit->text().replace(',', '.');
    int pos = 0;
    QValidator::State state = lineEdit->validator()->validate(numberStr, pos);
    if (state == QValidator::Acceptable)
        output = numberStr.toFloat(&ok);
    else
        return false;
    return ok;
}

void VWindowMain::simulationStartResult()
{
    if (ui->actionSlideshow->isChecked())
        startSlideshow();
    if (ui->actionVideo->isChecked())
    {
        startVideo();
    }
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
    m_pSlideshowShooter->stop();
    m_pVideoShooter->stop();
    ui->actionStart->setEnabled(true);
    ui->actionPause->setEnabled(false);
    ui->actionStop->setEnabled(false);
    activateSimControls(true);
}

void VWindowMain::activateSimControls(bool enabled)
{
    bool useTableParam = m_pFacade->isUsingTableParameters();
    ui->useTableParamCheckBox->setEnabled(enabled);
    ui->layerEnableCheckBox->setEnabled(enabled);
    ui->layerColorButton->setEnabled(enabled);
    ui->selectMaterialClothButton->setEnabled(enabled);
    ui->layerRemoveButton->setEnabled(enabled);
    ui->layerCutButton->setEnabled(enabled);
    ui->layerDuplicateButton->setEnabled(enabled);
    ui->layerEditButton->setEnabled(enabled);
    ui->addLayerButton->setEnabled(enabled);
    ui->sortLayersButton->setEnabled(enabled);
    ui->layerAngleEdit->setEnabled(enabled);
    ui->saveLayerAngleButton->setEnabled(enabled);
    ui->injectionPressureEdit->setEnabled(enabled);
    ui->saveInjectionPressureButton->setEnabled(enabled);
    ui->injectionDiameterEdit->setEnabled(enabled && !useTableParam);
    ui->injectionPlaceButton->setEnabled(enabled && !useTableParam);
    ui->resetInjectionDiameterButton->setEnabled(enabled && !useTableParam);
    ui->vacuumPressureEdit->setEnabled(enabled);
    ui->saveVacuumPressureButton->setEnabled(enabled);
    ui->vacuumDiameterEdit->setEnabled(enabled && !useTableParam);
    ui->vacuumPlaceButton->setEnabled(enabled && !useTableParam);
    ui->resetVacuumDiameterButton->setEnabled(enabled && !useTableParam);
    ui->selectMaterialResinButton->setEnabled(enabled);

    int layer = ui->layersTableWidget->currentRow();
    ui->layerUpButton->setEnabled(enabled && layer > 0);
    ui->layerDownButton->setEnabled(enabled && layer < static_cast<int>(m_pFacade->getLayersNumber()) - 1);
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
    VCloth::const_ptr cloth = m_pFacade->getMaterial(0);
    ui->layersTableWidget->insertRow(0);
    ui->layersTableWidget->setItem(0, 0, new QTableWidgetItem(m_pFacade->getLayerName(0)));
    ui->layersTableWidget->setItem(0, 1, new QTableWidgetItem(cloth->getName()));
    ui->layersTableWidget->setItem(0, 2, new QTableWidgetItem(QString::number(cloth->getAngleDeg())));
    ui->layersTableWidget->setItem(0, 3, new QTableWidgetItem());
    ui->layersTableWidget->item(0, 3)->setBackgroundColor(cloth->getBaseColor());
    ui->layersTableWidget->selectRow(0);
}

void VWindowMain::reloadLayersTable()
{
    ui->layersTableWidget->setRowCount(0);
    for (uint layer = 0; layer < m_pFacade->getLayersNumber(); ++layer)
    {
        VCloth::const_ptr cloth = m_pFacade->getMaterial(layer);
        ui->layersTableWidget->insertRow(layer);
        ui->layersTableWidget->setItem(layer, 0, new QTableWidgetItem(m_pFacade->getLayerName(layer)));
        ui->layersTableWidget->setItem(layer, 1, new QTableWidgetItem(cloth->getName()));
        ui->layersTableWidget->setItem(layer, 2, new QTableWidgetItem(QString::number(cloth->getAngleDeg())));
        ui->layersTableWidget->setItem(layer, 3, new QTableWidgetItem());
        ui->layersTableWidget->item(layer, 3)->setBackgroundColor(cloth->getBaseColor());
        markLayerAsEnabled(layer, m_pFacade->isLayerEnabled(layer));
        markLayerAsVisible(layer, m_pFacade->isLayerVisible(layer));
    }
    if (ui->layersTableWidget->rowCount() > 0)
        ui->layersTableWidget->selectRow(ui->layersTableWidget->rowCount() - 1);
    else
        ui->layerParamBox->setVisible(false);
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
        catch (VImportException &)
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
        catch (VExportException &)
        {
            QMessageBox::warning(this, ERROR_TITLE, EXPORT_TO_FILE_ERROR);
        }
        settings.setValue(QStringLiteral("import/lastDir"), lastDir);
        settings.sync();
    }
}

void VWindowMain::startCuttingLayer()
{
    if (ui->layersTableWidget->currentIndex().isValid())
    {
        uint layer = ui->layersTableWidget->currentRow();
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

void VWindowMain::saveWindowSettings()
{
    QSettings settings;
    settings.setValue("window/geometry", saveGeometry());
    settings.setValue("window/windowState", saveState());
    settings.setValue("window/splitter", ui->splitter->saveGeometry());
    settings.setValue("window/splitterState", ui->splitter->saveState());
    settings.setValue("window/simParamSplitter", ui->simParamSplitter->saveGeometry());
    settings.setValue("window/simParamSplitterState", ui->simParamSplitter->saveState());
    settings.sync();
}

void VWindowMain::loadWindowSettings()
{
    QSettings settings;
    restoreGeometry(settings.value("window/geometry").toByteArray());
    restoreState(settings.value("window/windowState").toByteArray());
    ui->splitter->restoreGeometry(settings.value("window/splitter").toByteArray());
    ui->splitter->restoreState(settings.value("window/splitterState").toByteArray());
    ui->simParamSplitter->restoreGeometry(settings.value("window/simParamSplitter").toByteArray());
    ui->simParamSplitter->restoreState(settings.value("window/simParamSplitterState").toByteArray());
}


void VWindowMain::setSavedState(bool saved)
{
    m_isSaved = saved;
    m_isSaved |= (m_pFacade->getNodesNumber() == 0);
    QString title = this->windowTitle();
    if (m_isSaved)
    {
        title.remove('*');
    }
    else if (!title.startsWith('*'))
    {
        title.insert(0, '*');
    }
    this->setWindowTitle(title);
}

void VWindowMain::showFilenameInTitle(const QString &filename)
{
    bool addStar = this->windowTitle().startsWith('*');
    QString title;
    if (addStar)
        title += '*';
    if (!filename.isEmpty())
    {
        QString baseFileName = QFileInfo(filename).fileName();
        title += baseFileName;
        title += QStringLiteral(" - ");
    }
    title += QCoreApplication::applicationName();
    this->setWindowTitle(title);
}

void VWindowMain::swapLayersCaptions(uint layer1, uint layer2)
{
    int columns{ui->layersTableWidget->columnCount()};
    std::vector<QTableWidgetItem*> items1(columns);
    std::vector<QTableWidgetItem*> items2(columns);
    for (int i{0}; i < columns; ++i)
    {
        items1.at(i) = ui->layersTableWidget->takeItem(layer1, i);
        items2.at(i) = ui->layersTableWidget->takeItem(layer2, i);
    }
    for (int i{0}; i < columns; ++i)
    {
        ui->layersTableWidget->setItem(layer2, i, items1.at(i));
        ui->layersTableWidget->setItem(layer1, i, items2.at(i));
    }
    ui->layersTableWidget->selectRow(layer2);
}

void VWindowMain::editLayer(uint layer)
{
    if (m_pFacade->isSimulationStopped())
    {
        if (m_pWindowPolygon == nullptr)
        {
            m_pWindowPolygon = new VWindowPolygon(this, m_pFacade);
            connect(m_pWindowPolygon, SIGNAL(polygonAvailable(const QPolygonF &, double, double)),
                    this, SLOT(m_on_got_polygon(const QPolygonF &, double, double)));
            connect(m_pWindowPolygon,SIGNAL(windowClosed()), this, SLOT(m_on_polygon_window_closed()));
        }
        m_pWindowPolygon->set1DMode(false);
        m_pWindowPolygon->setAngle(m_pFacade->getMaterial(layer)->getAngleDeg());
        const auto & polygons = m_pFacade->getPolygons(layer);
        if(polygons.size() > 0)
            m_pWindowPolygon->setPolygon(polygons.at(0));
        m_pWindowPolygon->show();
        m_pWindowPolygon->activateWindow();
    }
}

/**
  * Slideshow methods
  */

void VWindowMain::setSlideshowDir(const QString &dir)
{
    m_pSlideshowShooter->setDirPath(dir);
}

void VWindowMain::setSlideshowPeriod(float period)
{
    m_pSlideshowShooter->setPeriod(period);
}

void VWindowMain::setVideoDir(const QString &dir)
{
    m_pVideoShooter->setDirPath(dir);
}

void VWindowMain::setVideoFrequency(int frequency)
{
    m_pVideoShooter->setFrequency(frequency);
}

const QString & VWindowMain::getSlideshowDir() const
{
    return m_pSlideshowShooter->getDirPath();
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
    return m_pVideoShooter->getVideoFilePath();
}

int VWindowMain::getVideoFrequency() const
{
    return m_pVideoShooter->getFrequency();
}

void VWindowMain::startVideo()
{
    if (m_pVideoShooter->isSaving())
        addNewVideoShooter();
    m_pVideoShooter->start();
}

void VWindowMain::stopVideo()
{
    m_pVideoShooter->stop();
}

void VWindowMain::showSlideshowPeriod()
{
    float period = m_pSlideshowShooter->getPeriod();
    ui->slideshowPeriodSpinBox->setValue(period);
    ui->resetSlideshowPeriodButton->setEnabled(false);
}

void VWindowMain::showVideoFrequency()
{
    int frequency = m_pVideoShooter->getFrequency();
    ui->videoFrequencySpinBox->setValue(frequency);
    ui->resetVideoFrequencyButton->setEnabled(false);
}

void VWindowMain::loadShootersSettings()
{
    QSettings settings;
    QString slideshowDir, slideshowSuffixDir, videoDir, videoSuffixFile;
    float slideshowPeriod;
    int videoFrequency;

    slideshowDir = settings.value(QStringLiteral("slideshow/directory"), m_pSlideshowShooter->getDirPath()).toString();
    slideshowPeriod = settings.value(QStringLiteral("slideshow/period"), m_pSlideshowShooter->getPeriod()).toFloat();
    slideshowSuffixDir = settings.value(QStringLiteral("slideshow/suffixDirName"),
                                        m_pSlideshowShooter->getSuffixDirName()).toString();

    videoDir = settings.value(QStringLiteral("video/directory"), m_pVideoShooter->getDirPath()).toString();
    videoFrequency= settings.value(QStringLiteral("video/frequency"), m_pVideoShooter->getFrequency()).toInt();
    videoSuffixFile = settings.value(QStringLiteral("video/suffixFileName"),
                                     m_pVideoShooter->getSuffixFileName()).toString();
    m_pSlideshowShooter->setDirPath(slideshowDir);
    m_pSlideshowShooter->setPeriod(slideshowPeriod);
    m_pSlideshowShooter->setSuffixDirName(slideshowSuffixDir);
    m_pVideoShooter->setDirPath(videoDir);
    m_pVideoShooter->setFrequency(videoFrequency);
    m_pVideoShooter->setSuffixFileName(videoSuffixFile);
}

void VWindowMain::saveSootersSettings()
{
    QSettings settings;
    settings.setValue(QStringLiteral("slideshow/directory"), m_pSlideshowShooter->getDirPath());
    settings.setValue(QStringLiteral("slideshow/period"), m_pSlideshowShooter->getPeriod());
    settings.setValue(QStringLiteral("slideshow/suffixDirName"), m_pSlideshowShooter->getSuffixDirName());

    settings.setValue(QStringLiteral("video/directory"), m_pVideoShooter->getDirPath());
    settings.setValue(QStringLiteral("video/frequency"), m_pVideoShooter->getFrequency());
    settings.setValue(QStringLiteral("video/suffixFileName"), m_pVideoShooter->getSuffixFileName());
    settings.sync();
}

void VWindowMain::enableButtonsShootingWindows(bool enable)
{
    ui->simTab->setEnabled(enable);
    ui->appearanceTab->setEnabled(enable);
    ui->actionOpen->setEnabled(enable);
    ui->actionSave->setEnabled(enable);
}

/**
 * Slots
 */

void VWindowMain::m_on_layer_creation_from_file_available(const VCloth& material, const QString& filename,
                                                          const QString& layerName,
                                                          VLayerAbstractBuilder::VUnit units)
{
    addLayerFromFile(material, filename, layerName, units);
}

void VWindowMain::m_on_layer_creation_manual_available(const VCloth& material,const QPolygonF& polygon,
                                                       double characteristicLength,
                                                       const QString& layerName)
{
    addLayerFromPolygon(material, polygon, characteristicLength, layerName);
}

void VWindowMain::m_on_got_cloth(const QString & name, float cavityheight, float xpermeability, float ypermeability, float porosity)
{
    setCloth(name, cavityheight, xpermeability, ypermeability, porosity);
}

void VWindowMain::m_on_got_resin(const QString & name , float viscosity, float viscTempcoef,
                                 float lifetime, float lifetimeTempcoef)
{
    setResin(name, viscosity, viscTempcoef, lifetime, lifetimeTempcoef);
}

void VWindowMain::m_on_got_polygon(const QPolygonF & polygon, double angle, double characteristicLength)
{
    if (ui->layersTableWidget->currentIndex().isValid()
            && ui->layersTableWidget->currentRow() < int(m_pFacade->getLayersNumber()))
    {
        uint layer = ui->layersTableWidget->currentRow();
        try
        {
            m_pFacade->rebuildLayer(layer, angle, polygon, characteristicLength);
        }
        catch(VImportException &)
        {
            QMessageBox::warning(this, ERROR_TITLE, IMPORT_MANUAL_ERROR);
        }
    }
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

void VWindowMain::m_on_polygon_window_closed()
{
    deleteWindowPolygon();
}

void VWindowMain::m_on_layers_cleared()
{
    reloadLayersTable();
    showModelInfo();
    setSavedState(false);
}

void VWindowMain::m_on_video_saving_started()
{
}

void VWindowMain::m_on_video_saving_finished(bool result)
{
    if (!m_pVideoShooter->isWorking() && !m_pSlideshowShooter->isWorking())
    {
        if (result)
            QMessageBox::information(this, INFO_TITLE, SAVING_VIDEO_INFO.arg(m_pVideoShooter->getVideoFilePath()));
        else
            QMessageBox::warning(this, ERROR_TITLE, SAVING_VIDEO_ERROR);
        clearFinishedVideoShooters();
    }
}

void VWindowMain::on_addLayerButton_clicked()
{
    showWindowLayer();
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
    setSavedState(false);
}

void VWindowMain::m_on_material_changed(uint layer)
{
    updateLayerMaterialInfo(layer);
    setSavedState(false);
}

void VWindowMain::m_on_layer_name_changed(uint layer)
{
    updateLayerName(layer);
    setSavedState(false);
}

void VWindowMain::m_on_layer_enabled(uint layer, bool enable)
{
    markLayerAsEnabled(layer, enable);
    setSavedState(false);
}

void VWindowMain::m_on_layer_added()
{
    showNewLayer();
    setSavedState(false);
}

void VWindowMain::m_on_injection_point_set()
{
    injectionPointSelectionResult();
    setSavedState(false);
}

void VWindowMain::m_on_vacuum_point_set()
{
    vacuumPointSelectionResult();
    setSavedState(false);
}

void VWindowMain::m_on_simutation_started()
{
    simulationStartResult();
    setSavedState(false);
}

void VWindowMain::m_on_simutation_paused()
{
    simulationPauseResult();
}

void VWindowMain::m_on_simutation_stopped()
{
    simulationStopResult();
    setSavedState(false);
}

void VWindowMain::m_on_resin_changed()
{
    updateResinInfo();
    setSavedState(false);
}

void VWindowMain::m_on_injection_diameter_set(float)
{
    showInjectionDiameter();
    setSavedState(false);
}

void VWindowMain::m_on_vacuum_diameter_set(float)
{
    showVacuumDiameter();
    setSavedState(false);
}

void VWindowMain::m_on_temperature_set(double)
{
    showTemperature();
    setSavedState(false);
}

void VWindowMain::m_on_vacuum_pressure_set(double)
{
    showVacuumPressure();
    setSavedState(false);
}

void VWindowMain::m_on_injection_pressure_set(double)
{
    showInjectionPressure();
    setSavedState(false);
}

void VWindowMain::m_on_time_limit_set(double)
{
    showTimeLimit();
    setSavedState(false);
}

void VWindowMain::m_on_time_limit_mode_switched(bool on)
{
    ui->timeLimitCheckBox->setChecked(on);
    setSavedState(false);
}

void VWindowMain::m_on_lifetime_consideration_switched(bool on)
{
    ui->timeConsiderationCheckbox->setChecked(on);
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
    setSavedState(false);
}

void VWindowMain::m_on_model_loaded()
{
    reloadLayersTable();
    setSavedState(true);
}

void VWindowMain::m_on_model_saved()
{
    setSavedState(true);
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
    setSavedState(false);
}

void VWindowMain::m_on_selection_enabled(bool checked)
{
    ui->layerCutButton->setChecked(checked);
}

void VWindowMain::m_on_cube_side_changed(float)
{
    showCubeSide();
}

void VWindowMain::m_on_filename_changed(const QString & filename)
{
    showFilenameInTitle(filename);
}

void VWindowMain::m_on_layers_swapped(uint layer1, uint layer2)
{
    swapLayersCaptions(layer1, layer2);
}

void VWindowMain::m_on_slideshow_started()
{
    ui->slideshowBox->setEnabled(false);
#ifndef __linux__
    setWindowFlags((windowFlags() & ~Qt::WindowStaysOnBottomHint) | ON_TOP_FLAGS);
    show();
#endif
}

void VWindowMain::m_on_slideshow_stopped()
{
    ui->slideshowBox->setEnabled(true);
#ifndef __linux__
    if (!m_pVideoShooter->isWorking())
    {
        setWindowFlags((windowFlags() & ~ON_TOP_FLAGS) | Qt::WindowStaysOnBottomHint);
        show();
    }
#endif
}

void VWindowMain::m_on_slideshow_directory_changed()
{
    const QString& dir = m_pSlideshowShooter->getDirPath();
    ui->slideshowDirLabel->setText(dir);
}

void VWindowMain::m_on_slideshow_period_changed()
{
    showSlideshowPeriod();
}

void VWindowMain::m_on_slideshow_suffix_dirname_changed()
{
    const QString &dirname = m_pSlideshowShooter->getSuffixDirName();
    ui->slideshowDirNameEdit->setText(dirname);
    ui->resetSlideshowDirNameButton->setEnabled(false);
}

void VWindowMain::m_on_video_started()
{
    ui->videoBox->setEnabled(false);
#ifndef __linux__
    setWindowFlags((windowFlags() & ~Qt::WindowStaysOnBottomHint) | ON_TOP_FLAGS);
    show();
#endif
}

void VWindowMain::m_on_video_stopped()
{
    ui->videoBox->setEnabled(true);
#ifndef __linux__
    if (!m_pSlideshowShooter->isWorking())
    {
        setWindowFlags((windowFlags() & ~ON_TOP_FLAGS) | Qt::WindowStaysOnBottomHint);
        show();
    }
#endif
}

void VWindowMain::m_on_video_directory_changed()
{
    const QString& dir = m_pVideoShooter->getDirPath();
    ui->videoDirLabel->setText(dir);
}

void VWindowMain::m_on_video_frequency_changed()
{
    showVideoFrequency();
}

void VWindowMain::m_on_video_suffix_filename_changed()
{
    const QString &filename = m_pVideoShooter->getSuffixFileName();
    ui->videoFileNameEdit->setText(filename);
    ui->resetVideoFileNameButton->setEnabled(false);
}


void VWindowMain::m_on_table_size_set(float width, float height)
{
    ui->tableXSpinBox->setValue(width);
    ui->tableYSpinBox->setValue(height);
    ui->resetTableSizeButton->setEnabled(false);
    setSavedState(false);
}

void VWindowMain::m_on_table_injection_coords_set(float x, float y)
{
    ui->tableInjectionXSpinBox->setValue(x);
    ui->tableInjectionYSpinBox->setValue(y);
    ui->resetTableInjectionCoordsButton->setEnabled(false);
    setSavedState(false);
}

void VWindowMain::m_on_table_vacuum_coords_set(float x, float y)
{
    ui->tableVacuumXSpinBox->setValue(x);
    ui->tableVacuumYSpinBox->setValue(y);
    ui->resetTableVacuumCoordsButton->setEnabled(false);
    setSavedState(false);
}

void VWindowMain::m_on_table_injection_diameter_set(float diameter)
{
    ui->tableInjectionDiameterSpinBox->setValue(diameter);
    ui->resetTableInjectionDiameterButton->setEnabled(false);
    setSavedState(false);
}

void VWindowMain::m_on_table_vacuum_diameter_set(float diameter)
{
    ui->tableVacuumDiameterSpinBox->setValue(diameter);
    ui->resetTableVacuumDiameterButton->setEnabled(false);
    setSavedState(false);
}

void VWindowMain::m_on_use_table_parameters_set(bool use)
{
    if (ui->useTableParamCheckBox->isChecked() != use)
        ui->useTableParamCheckBox->setChecked(use);
    bool enabled = !(m_pFacade->isSimulationActive());
    ui->injectionDiameterEdit->setEnabled(enabled && !use);
    ui->injectionPlaceButton->setEnabled(enabled && !use);
    ui->resetInjectionDiameterButton->setEnabled(enabled && !use);
    ui->vacuumDiameterEdit->setEnabled(enabled && !use);
    ui->vacuumPlaceButton->setEnabled(enabled && !use);
    ui->resetVacuumDiameterButton->setEnabled(enabled && !use);
    setSavedState(false);
}

void VWindowMain::m_stop_on_vacuum_full_switched(bool enabled)
{
    if (ui->vacuumFullLimitCheckBox->isChecked() != enabled)
        ui->vacuumFullLimitCheckBox->setChecked(enabled);
}

void VWindowMain::m_on_additional_controls_enabled(bool enabled)
{
    if (ui->additionalOptionsCheckBox->isChecked() != enabled)
        ui->additionalOptionsCheckBox->setChecked(enabled);
    ui->layerCutButton->setVisible(enabled);
}

void VWindowMain::m_on_layer_rebuilt(uint layer)
{
    if (static_cast<int>(layer) < ui->layersTableWidget->rowCount())
    {
        ui->layersTableWidget->selectRow(layer);
        updateLayerMaterialInfo(layer);
    }
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
    if(!checked)
        stopSlideshow();
    else if (!m_pFacade->isSimulationStopped())
        startSlideshow();
}

void VWindowMain::on_actionVideo_triggered(bool checked)
{
    if (!checked)
        stopVideo();
    else if (!m_pFacade->isSimulationStopped())
        startVideo();
}

void VWindowMain::on_chooseSlideshowDirButton_clicked()
{
    m_pFacade->pauseSimulation();
    QString lastDir = m_pSlideshowShooter->getDirPath();
    QString dirName = QFileDialog::getExistingDirectory(this, SLIDESHOW_DIR_DIALOG_TITLE, lastDir);
    QDir parentDir(dirName);
    if (!dirName.isEmpty() && parentDir.exists())
    {
        setSlideshowDir(dirName);
    }
}

void VWindowMain::on_slideshowPeriodSpinBox_valueChanged(double)
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
    setSlideshowPeriod(value);
}

void VWindowMain::on_chooseVideoDirButton_clicked()
{
    m_pFacade->pauseSimulation();
    QString lastDir = m_pVideoShooter->getDirPath();
    QString dirName = QFileDialog::getExistingDirectory(this, VIDEO_DIR_DIALOG_TITLE, lastDir);
    QDir parentDir(dirName);
    if (!dirName.isEmpty() && parentDir.exists())
    {
        setVideoDir(dirName);
    }
}

void VWindowMain::on_resetVideoFrequencyButton_clicked()
{
    showVideoFrequency();
}

void VWindowMain::on_saveVideoFrequencyButton_clicked()
{
    int value = ui->videoFrequencySpinBox->value();
    setVideoFrequency(value);
}

void VWindowMain::on_videoFrequencySpinBox_valueChanged(int)
{
    ui->resetVideoFrequencyButton->setEnabled(true);
}

void VWindowMain::on_slideshowDirNameEdit_textEdited(const QString &arg1)
{
    ui->resetSlideshowDirNameButton->setEnabled(true);
}

void VWindowMain::on_resetSlideshowDirNameButton_clicked()
{
    ui->slideshowDirNameEdit->setText(m_pSlideshowShooter->getSuffixDirName());
}

void VWindowMain::on_saveSlideshowDirNameButton_clicked()
{
    m_pSlideshowShooter->setSuffixDirName(ui->slideshowDirNameEdit->text());
}

void VWindowMain::on_videoFileNameEdit_textEdited(const QString &arg1)
{
    ui->resetVideoFileNameButton->setEnabled(true);
}

void VWindowMain::on_resetVideoFileNameButton_clicked()
{
    ui->videoFileNameEdit->setText(m_pVideoShooter->getSuffixFileName());
}

void VWindowMain::on_saveVideoFileNameButton_clicked()
{
    m_pVideoShooter->setSuffixFileName(ui->videoFileNameEdit->text());
}

void VWindowMain::on_layerUpButton_clicked()
{
    if (ui->layersTableWidget->currentIndex().isValid()
            && ui->layersTableWidget->currentRow() < int(m_pFacade->getLayersNumber()))
    {
        uint layer = ui->layersTableWidget->currentRow();
        m_pFacade->moveLayerUp(layer);
    }
}

void VWindowMain::on_layerDownButton_clicked()
{
    if (ui->layersTableWidget->currentIndex().isValid()
            && ui->layersTableWidget->currentRow() < int(m_pFacade->getLayersNumber()))
    {
        uint layer = ui->layersTableWidget->currentRow();
        m_pFacade->moveLayerDown(layer);
    }
}

void VWindowMain::on_sortLayersButton_clicked()
{
    m_pFacade->sortLayers();
}

void VWindowMain::on_useTableParamCheckBox_clicked(bool checked)
{
    m_pFacade->useTableParameters(checked);
}

void VWindowMain::on_tableXSpinBox_valueChanged(double)
{
    const QVector2D & size = m_pFacade->getTable()->getSize();
    float x = static_cast<float>(ui->tableXSpinBox->value());
    float y = static_cast<float>(ui->tableYSpinBox->value());
    bool enable = (x != size.x() || y != size.y());
    ui->resetTableSizeButton->setEnabled(enable);
}

void VWindowMain::on_tableYSpinBox_valueChanged(double)
{
    const QVector2D & size = m_pFacade->getTable()->getSize();
    float x = static_cast<float>(ui->tableXSpinBox->value());
    float y = static_cast<float>(ui->tableYSpinBox->value());
    bool enable = (x != size.x() || y != size.y());
    ui->resetTableSizeButton->setEnabled(enable);
}

void VWindowMain::on_tableInjectionXSpinBox_valueChanged(double)
{
    const QVector2D & coords = m_pFacade->getTable()->getInjectionCoords();
    float x = static_cast<float>(ui->tableInjectionXSpinBox->value());
    float y = static_cast<float>(ui->tableInjectionYSpinBox->value());
    bool enable = (x != coords.x() || y != coords.y());
    ui->resetTableInjectionCoordsButton->setEnabled(enable);
}

void VWindowMain::on_tableInjectionYSpinBox_valueChanged(double)
{
    const QVector2D & coords = m_pFacade->getTable()->getInjectionCoords();
    float x = static_cast<float>(ui->tableInjectionXSpinBox->value());
    float y = static_cast<float>(ui->tableInjectionYSpinBox->value());
    bool enable = (x != coords.x() || y != coords.y());
    ui->resetTableInjectionCoordsButton->setEnabled(enable);
}

void VWindowMain::on_tableInjectionDiameterSpinBox_valueChanged(double arg1)
{
    bool enable = (static_cast<float>(arg1) != m_pFacade->getTable()->getInjectionDiameter());
    ui->resetTableInjectionDiameterButton->setEnabled(enable);
}

void VWindowMain::on_tableVacuumXSpinBox_valueChanged(double)
{
    const QVector2D & coords = m_pFacade->getTable()->getVacuumCoords();
    float x = static_cast<float>(ui->tableVacuumXSpinBox->value());
    float y = static_cast<float>(ui->tableVacuumYSpinBox->value());
    bool enable = (x != coords.x() || y != coords.y());
    ui->resetTableVacuumCoordsButton->setEnabled(enable);
}

void VWindowMain::on_tableVacuumYSpinBox_valueChanged(double)
{
    const QVector2D & coords = m_pFacade->getTable()->getVacuumCoords();
    float x = static_cast<float>(ui->tableVacuumXSpinBox->value());
    float y = static_cast<float>(ui->tableVacuumYSpinBox->value());
    bool enable = (x != coords.x() || y != coords.y());
    ui->resetTableVacuumCoordsButton->setEnabled(enable);
}

void VWindowMain::on_tableVacuumDiameterSpinBox_valueChanged(double arg1)
{
    bool enable = (static_cast<float>(arg1) != m_pFacade->getTable()->getVacuumDiameter());
    ui->resetTableVacuumDiameterButton->setEnabled(enable);
}

void VWindowMain::on_resetTableSizeButton_clicked()
{
    const QVector2D & coords = m_pFacade->getTable()->getSize();
    ui->tableXSpinBox->setValue(coords.x());
    ui->tableYSpinBox->setValue(coords.y());
}

void VWindowMain::on_saveTableSizeButton_clicked()
{
    m_pFacade->setTableSize(ui->tableXSpinBox->value(), ui->tableYSpinBox->value());
}

void VWindowMain::on_resetTableInjectionCoordsButton_clicked()
{
    const QVector2D & coords = m_pFacade->getTable()->getInjectionCoords();
    ui->tableInjectionXSpinBox->setValue(coords.x());
    ui->tableInjectionYSpinBox->setValue(coords.y());
}

void VWindowMain::on_saveTableInjectionCoordsButton_clicked()
{
    m_pFacade->setTableInjectionCoords(ui->tableInjectionXSpinBox->value(),
                                       ui->tableInjectionYSpinBox->value());
}

void VWindowMain::on_resetTableInjectionDiameterButton_clicked()
{
    float diameter = m_pFacade->getTable()->getInjectionDiameter();
    ui->tableInjectionDiameterSpinBox->setValue(diameter);
}

void VWindowMain::on_saveTableInjectionDiameterButton_clicked()
{
    m_pFacade->setTableInjectionDiameter(ui->tableInjectionDiameterSpinBox->value());
}

void VWindowMain::on_resetTableVacuumCoordsButton_clicked()
{
    const QVector2D & coords = m_pFacade->getTable()->getVacuumCoords();
    ui->tableVacuumXSpinBox->setValue(coords.x());
    ui->tableVacuumYSpinBox->setValue(coords.y());
}

void VWindowMain::on_saveTableVacuumCoordsButton_clicked()
{
    m_pFacade->setTableVacuumCoords(ui->tableVacuumXSpinBox->value(),
                                    ui->tableVacuumYSpinBox->value());
}

void VWindowMain::on_resetTableVacuumDiameterButton_clicked()
{
    float diameter = m_pFacade->getTable()->getVacuumDiameter();
    ui->tableVacuumDiameterSpinBox->setValue(diameter);
}

void VWindowMain::on_saveTableVacuumDiameterButton_clicked()
{
    m_pFacade->setTableVacuumDiameter(ui->tableVacuumDiameterSpinBox->value());
}

void VWindowMain::on_timeConsiderationCheckbox_clicked(bool checked)
{
    m_pFacade->considerLifetime(checked);
}

void VWindowMain::on_actionFolderSlideshow_triggered()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_pSlideshowShooter->getDirPath()));
}

void VWindowMain::on_actionFolderVideo_triggered()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_pVideoShooter->getDirPath()));
}

void VWindowMain::on_layersTableWidget_itemSelectionChanged()
{
    selectLayer();
}

void VWindowMain::on_layerNameEdit_textEdited(const QString &)
{
    ui->resetLayerNameButton->setEnabled(true);
}

void VWindowMain::on_resetLayerNameButton_clicked()
{
    if (ui->layersTableWidget->currentIndex().isValid()
            && ui->layersTableWidget->currentRow() < int(m_pFacade->getLayersNumber()))
    {
        uint layer = ui->layersTableWidget->currentRow();
        ui->layerNameEdit->setText(m_pFacade->getLayerName(layer));
        ui->resetLayerNameButton->setEnabled(false);
    }
}

void VWindowMain::on_saveLayerNameButton_clicked()
{
    if (ui->layersTableWidget->currentIndex().isValid()
            && ui->layersTableWidget->currentRow() < int(m_pFacade->getLayersNumber()))
    {
        uint layer = ui->layersTableWidget->currentRow();
        m_pFacade->setLayerName(layer, ui->layerNameEdit->text());
    }
}

void VWindowMain::on_vacuumFullLimitCheckBox_clicked(bool checked)
{
    m_pFacade->stopOnVacuumFull(checked);
}

void VWindowMain::on_additionalOptionsCheckBox_clicked(bool checked)
{
    m_pFacade->enableAdditionalGraphicsControls(checked);
}

void VWindowMain::on_layerDuplicateButton_clicked()
{
    if (ui->layersTableWidget->currentIndex().isValid()
            && ui->layersTableWidget->currentRow() < int(m_pFacade->getLayersNumber()))
    {
        uint layer = ui->layersTableWidget->currentRow();
        try
        {
            m_pFacade->duplicateLayer(layer);
        }
        catch(VImportException &)
        {
            QMessageBox::warning(this, ERROR_TITLE, IMPORT_MANUAL_ERROR);
        }
    }
}

void VWindowMain::on_layerEditButton_clicked()
{
    if (ui->layersTableWidget->currentIndex().isValid()
            && ui->layersTableWidget->currentRow() < int(m_pFacade->getLayersNumber()))
    {
        uint layer = ui->layersTableWidget->currentRow();
        editLayer(layer);
    }
}

void VWindowMain::on_layersTableWidget_doubleClicked(const QModelIndex &index)
{
    editLayer(index.row());
}

void VWindowMain::on_layerAngleEdit_textChanged(const QString &)
{
    ui->resetLayerAngleButton->setEnabled(true);
}

void VWindowMain::on_saveLayerAngleButton_clicked()
{
    saveAngle();
}

void VWindowMain::on_resetLayerAngleButton_clicked()
{
    showAngle();
}
