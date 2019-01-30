/**
 * Project VARI
 * @author Alexey Barashkov
 */
#ifdef DEBUG_MODE
#include <QDebug>
#endif
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QStyleFactory>
#include <functional>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoExtSelection.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoLight.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/manips/SoCenterballManip.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/Qt/widgets/SoQtThumbWheel.h>

#include "VGraphicsViewer.h"
#include "graphics_elements/VGraphicsNode.h"
/**
 * VGraphicsViewer implementation
 */

const QString VGraphicsViewer::LEFT_WHEEL_CAPTION("X");
const QString VGraphicsViewer::RIGHT_WHEEL_CAPTION("Масштаб");
const QString VGraphicsViewer::BOTTOM_WHEEL_CAPTION("Y");
const QString VGraphicsViewer::REAL_TIME_LABEL_CAPTION("Время моделирования (с):");
const QString VGraphicsViewer::SIM_TIME_LABEL_CAPTION("Время процесса (с):");
const QString VGraphicsViewer::REALTIME_FACTOR_LABEL_CAPTION("Фактор реального времни:");
const QString VGraphicsViewer::ITERATION_LABEL_CAPTION("Номер итерации:");
const QString VGraphicsViewer::FILLED_PERCENT_LABEL_CAPTION("Готовность эксперимента (%):");
const QString VGraphicsViewer::AVERAGE_PRESSURE_LABEL_CAPTION("Среднее давление (Па):");

const int VGraphicsViewer::ICON_SIZE = 24;


VGraphicsViewer::VGraphicsViewer(QWidget *parent, const VSimulator::ptr &simulator) :
    SoQtExaminerViewer (parent, nullptr, true, SoQtFullViewer::BUILD_ALL, SoQtFullViewer::BROWSER, false),
    m_pSimulator(simulator),
    m_pRoot(new SoSeparator),
    m_pFigureRoot(new SoSeparator),
    m_pShapeHints(new SoShapeHints),
    m_pCam(new SoOrthographicCamera),
    m_pSelection(new SoExtSelection),
    m_pOldSelection(nullptr),
    m_pTransformBox(nullptr),
    m_pSelectedPath(nullptr),
    m_renderStopFlag(false),
    m_interactionMode(PICK),
    m_dragCanceled(false),
    m_pSelectedNodesIds(new std::vector<uint>),
    m_pTransformedNodesCoords(new std::vector<std::pair<uint, QVector3D> >)
{
    m_pRoot->ref();
    m_pFigureRoot->ref();
    m_pShapeHints->ref();
    m_pCam->ref();
    m_pSelection->ref();
    setAntialiasing(true, 1);
    setBackgroundColor(SbColor(0.7f, 0.7f, 0.7f));
    setPopupMenuEnabled(false);

    m_pBaseWidget = buildWidget(getParentWidget());
    setBaseWidget(m_pBaseWidget);

    SoEventCallback * cb = new SoEventCallback;
    cb->addEventCallback(SoMouseButtonEvent::getClassTypeId(), event_cb, this);
    m_pRoot->insertChild(cb, 0);

    m_pShapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    m_pRoot->addChild(m_pShapeHints);
    m_pRoot->addChild(m_pCam);

    initSelection();
    m_pRoot->addChild(m_pSelection);

    setSceneGraph(m_pRoot);

    connect(this, SIGNAL(askForRender()), this, SLOT(doRender()));
    connect(this, SIGNAL(askForDisplayingInfo()), this, SLOT(displayInfo()));
    m_pRenderWaiterThread.reset(new std::thread(std::bind(&VGraphicsViewer::process, this)));
    m_renderSuccessNotifier.notifyOne();
}

void VGraphicsViewer::initSelection()
{
    m_pSelection->lassoPolicy = SoExtSelection::PART;
    m_pSelection->lassoMode = SoExtSelection::ALL_SHAPES;
    m_pSelection->policy = SoSelection::SINGLE;
    m_pSelection->lassoType = SoExtSelection::NOLASSO;

    m_pSelection->setLassoColor(SbColor(0.9f,0.7f,0.2f));
    m_pSelection->setLassoWidth(3);
    m_pSelection->setOverlayLassoPattern(0xf0f0);
    m_pSelection->animateOverlayLasso(false);
    m_pSelection->addFinishCallback(selection_finish_cb, this);
    m_pSelection->addSelectionCallback(selection_cb, this);
    m_pSelection->addDeselectionCallback(deselection_cb, this);

    m_pSelection->addChild(m_pFigureRoot);
}

VGraphicsViewer::~VGraphicsViewer()
{
    stopRender();
    m_pRenderWaiterThread->join();
    m_pRenderWaiterThread.reset();
    m_dragCanceled = true;
    m_pSelection->deselectAll();
    m_pFigureRoot->removeAllChildren();
    m_pRoot->removeAllChildren();
    setSceneGraph(nullptr);
    setCamera(nullptr);
	if (m_pTransformBox != nullptr)
		m_pTransformBox->unref();
	if (m_pSelectedPath != nullptr)
		m_pSelectedPath->unref();
    if (m_pOldSelection != nullptr)
        m_pOldSelection->unref();
    m_pRoot->unref();
    m_pFigureRoot->unref();
    m_pShapeHints->unref();
    m_pCam->unref();
    m_pSelection->unref();
    delete m_pBaseWidget;
}

QWidget* VGraphicsViewer::buildLeftTrim(QWidget * parent)
{
    const int WIDGET_WIDTH = 45;
    const int HORIZONTAL_SPACING = 4;
    QWidget * widget = new QWidget(parent);
    widget->setFixedWidth(WIDGET_WIDTH);

    QGridLayout * layout = new QGridLayout(widget);

    SoQtThumbWheel * wheel = new SoQtThumbWheel(SoQtThumbWheel::Vertical, widget);
    wheel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->leftWheel = wheel;
    wheel->setRangeBoundaryHandling(SoQtThumbWheel::ACCUMULATE);
    this->leftWheelVal = wheel->value();

    QObject::connect(wheel, SIGNAL(wheelMoved(float)), this, SLOT(leftWheelChanged(float)));
    QObject::connect(wheel, SIGNAL(wheelPressed()), this, SLOT(leftWheelPressed()));
    QObject::connect(wheel, SIGNAL(wheelReleased()), this, SLOT(leftWheelReleased()));

    QLabel * colorScaleLabel = new QLabel(widget);
    colorScaleLabel->setPixmap(QPixmap(QStringLiteral(":/img/colormap.png")));

    layout->addItem(new QSpacerItem(0,0,QSizePolicy::Minimum,QSizePolicy::MinimumExpanding),0, 0);
    layout->addWidget(colorScaleLabel, 1, 0, Qt::AlignCenter | Qt::AlignHCenter);
    layout->addItem(new QSpacerItem(0,0,QSizePolicy::Minimum,QSizePolicy::MinimumExpanding),2, 0);
    layout->addWidget(wheel, 3, 0, Qt::AlignBottom | Qt::AlignHCenter);
    layout->setHorizontalSpacing(HORIZONTAL_SPACING);
    layout->setContentsMargins(0,0,0,0);
    layout->activate();

    return widget;
}

QWidget* VGraphicsViewer::buildBottomTrim(QWidget * parent)
{
    const int WIDGET_HEIGHT = 90;
    const int INFO_LABEL_WIDHT = 68;
    const int VERTICAL_SPACING = 4;
    const int LEFT_MARGIN = 15;
    QWidget * widget = new QWidget(parent);
    widget->setFixedHeight(WIDGET_HEIGHT);

    QGridLayout * layout = new QGridLayout(widget);

    SoQtThumbWheel * wheel = new SoQtThumbWheel(SoQtThumbWheel::Horizontal, widget);
    wheel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->bottomWheel = wheel;
    wheel->setRangeBoundaryHandling(SoQtThumbWheel::ACCUMULATE);
    this->bottomWheelVal = wheel->value();

    QObject::connect(wheel, SIGNAL(wheelMoved(float)), this, SLOT(bottomWheelChanged(float)));
    QObject::connect(wheel, SIGNAL(wheelPressed()), this, SLOT(bottomWheelPressed()));
    QObject::connect(wheel, SIGNAL(wheelReleased()), this, SLOT(bottomWheelReleased()));

    QLabel * leftWheelCaptionLabel = new QLabel(widget);
    QLabel * rightWheelCaptionLabel = new QLabel(widget);
    QLabel * bottomWheelCaptionLabel = new QLabel(widget);
    QLabel * realTimeCaptionLabel = new QLabel(widget);
    QLabel * simTimeCaptionLabel = new QLabel(widget);
    QLabel * realtimeFactorCaptionLabel = new QLabel(widget);
    QLabel * iterationCaptionLabel = new QLabel(widget);
    QLabel * filledPercentCaptionLabel = new QLabel(widget);
    QLabel * averagePressureCaptionLabel = new QLabel(widget);

    m_pRealTimeLabel = new QLabel(widget);
    m_pSimTimeLabel = new QLabel(widget);
    m_pRealtimeFactorLabel = new QLabel(widget);
    m_pIterationLabel = new QLabel(widget);
    m_pFilledPercentLabel = new QLabel(widget);
    m_pAveragePressureLabel = new QLabel(widget);

    m_pRealTimeLabel->setFixedWidth(INFO_LABEL_WIDHT);
    m_pSimTimeLabel->setFixedWidth(INFO_LABEL_WIDHT);
    m_pRealtimeFactorLabel->setFixedWidth(INFO_LABEL_WIDHT);
    m_pIterationLabel->setFixedWidth(INFO_LABEL_WIDHT);
    m_pFilledPercentLabel->setFixedWidth(INFO_LABEL_WIDHT);
    m_pAveragePressureLabel->setFixedWidth(INFO_LABEL_WIDHT);

    leftWheelCaptionLabel->setText(LEFT_WHEEL_CAPTION);
    rightWheelCaptionLabel->setText(RIGHT_WHEEL_CAPTION);
    bottomWheelCaptionLabel->setText(BOTTOM_WHEEL_CAPTION);
    realTimeCaptionLabel->setText(REAL_TIME_LABEL_CAPTION);
    simTimeCaptionLabel->setText(SIM_TIME_LABEL_CAPTION);
    realtimeFactorCaptionLabel->setText(REALTIME_FACTOR_LABEL_CAPTION);
    iterationCaptionLabel->setText(ITERATION_LABEL_CAPTION);
    filledPercentCaptionLabel->setText(FILLED_PERCENT_LABEL_CAPTION);
    averagePressureCaptionLabel->setText(AVERAGE_PRESSURE_LABEL_CAPTION);

    layout->addWidget(leftWheelCaptionLabel, 0, 0, Qt::AlignBottom | Qt::AlignCenter);
    layout->addItem(new QSpacerItem(0,0,QSizePolicy::MinimumExpanding,QSizePolicy::Minimum),0, 1);
    layout->addWidget(wheel, 0, 2, Qt::AlignBottom | Qt::AlignLeft);
    layout->addWidget(bottomWheelCaptionLabel, 1, 2, Qt::AlignBottom | Qt::AlignHCenter);
    layout->addItem(new QSpacerItem(0,0,QSizePolicy::MinimumExpanding,QSizePolicy::Minimum),0, 3);
    layout->addWidget(realTimeCaptionLabel, 0, 4, Qt::AlignBottom | Qt::AlignRight);
    layout->addWidget(m_pRealTimeLabel, 0, 5, Qt::AlignBottom | Qt::AlignLeft);
    layout->addWidget(realtimeFactorCaptionLabel, 1, 4, Qt::AlignBottom | Qt::AlignRight);
    layout->addWidget(m_pRealtimeFactorLabel, 1, 5, Qt::AlignBottom | Qt::AlignLeft);
    layout->addWidget(simTimeCaptionLabel, 0, 6, Qt::AlignBottom | Qt::AlignRight);
    layout->addWidget(m_pSimTimeLabel, 0, 7, Qt::AlignBottom | Qt::AlignLeft);
    layout->addWidget(iterationCaptionLabel, 1, 6, Qt::AlignBottom | Qt::AlignRight);
    layout->addWidget(m_pIterationLabel, 1, 7, Qt::AlignBottom | Qt::AlignLeft);
    layout->addWidget(filledPercentCaptionLabel, 0, 8, Qt::AlignBottom | Qt::AlignRight);
    layout->addWidget(m_pFilledPercentLabel, 0, 9, Qt::AlignBottom | Qt::AlignLeft);
    layout->addWidget(averagePressureCaptionLabel, 1, 8, Qt::AlignBottom | Qt::AlignRight);
    layout->addWidget(m_pAveragePressureLabel, 1, 9, Qt::AlignBottom | Qt::AlignLeft);
    layout->addItem(new QSpacerItem(0,0,QSizePolicy::MinimumExpanding,QSizePolicy::Minimum),0, 10);
    layout->addWidget(rightWheelCaptionLabel, 0, 11, Qt::AlignBottom | Qt::AlignRight);

    layout->setVerticalSpacing(VERTICAL_SPACING);
    layout->setContentsMargins(LEFT_MARGIN,0,0,0);
    layout->setSizeConstraint(QLayout::SetMinimumSize);

    layout->activate();

    return widget;
}

void VGraphicsViewer::createViewerButtons(QWidget * parent, SbPList * buttonlist)
{
    SoQtExaminerViewer::createViewerButtons(parent, buttonlist);

    m_pXYButton = new QPushButton(QIcon(QStringLiteral(":/img/xyplane.png")),
                                   QStringLiteral(""), parent);
    m_pXYButton->setFocusPolicy(Qt::NoFocus);
    m_pXYButton->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_pXYButton->adjustSize();
    connect(m_pXYButton, SIGNAL(clicked()), this, SLOT(viewFromAbove()));
    buttonlist->append(m_pXYButton);

    m_pYZButton = new QPushButton(QIcon(QStringLiteral(":/img/yzplane.png")),
                                   QStringLiteral(""), parent);
    m_pYZButton->setFocusPolicy(Qt::NoFocus);
    m_pYZButton->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_pYZButton->adjustSize();
    connect(m_pYZButton, SIGNAL(clicked()), this, SLOT(viewFromRight()));
    buttonlist->append(m_pYZButton);

    m_pDragButton = new QPushButton(QIcon(QStringLiteral(":/img/drag.png")),
                                                        QStringLiteral(""), parent);
    m_pDragButton->setFocusPolicy(Qt::NoFocus);
    m_pDragButton->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_pDragButton->adjustSize();
    m_pDragButton->setCheckable(true);
    m_pDragButton->setChecked(false);
    m_pDragButton->setVisible(true);
    connect(m_pDragButton, SIGNAL(toggled(bool)), this, SLOT(dragModeSwitch(bool)));
    buttonlist->append(m_pDragButton);

    m_pSelectionButton = new QPushButton(QIcon(QStringLiteral(":/img/cut.png")),
                                                        QStringLiteral(""), parent);
    m_pSelectionButton->setFocusPolicy(Qt::NoFocus);
    m_pSelectionButton->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_pSelectionButton->adjustSize();
    m_pSelectionButton->setCheckable(true);
    m_pSelectionButton->setChecked(false);
    m_pSelectionButton->setVisible(false);
    connect(m_pSelectionButton, SIGNAL(toggled(bool)), this, SLOT(selectionModeSwitch(bool)));
    buttonlist->append(m_pSelectionButton);

#if (defined Q_OS_MAC)
    QStyle * style = QStyleFactory::create(QStringLiteral("windows"));
    if (style)
    {
        m_pXYButton->setStyle(style);
        m_pYZButton->setStyle(style);
        m_pDragButton->setStyle(style);
        m_pSelectionButton->setStyle(style);
    }
#endif
}

void VGraphicsViewer::setGraphicsElements(const std::vector<VLayer::const_ptr> &layers)
{
    clearAll();
    for (uint i = 0; i < layers.size(); ++i)
    {
        if (layers.at(i)->isActive())
            m_graphicsLayers.push_back(new VGraphicsLayer(layers.at(i), i));
    }
    {
        std::lock_guard<std::recursive_mutex> locker(m_graphMutex);
        for (auto layer: m_graphicsLayers)
            m_pFigureRoot->addChild(layer);
    }
}

void VGraphicsViewer::updateColors()
{
    updateTriangleColors();
    updateNodeColors();
}

void VGraphicsViewer::updateTriangleColors()
{
    std::lock_guard<std::mutex> lock(*m_pTrianglesLock);
    for (auto layer : m_graphicsLayers)
        layer->updateTriangleColors();
}

void VGraphicsViewer::updateNodeColors()
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    for (auto layer : m_graphicsLayers)
        layer->updateNodeColors();
}

void VGraphicsViewer::updateColors(uint layerNumber)
{
    updateTriangleColors(layerNumber);
    updateNodeColors(layerNumber);
}

void VGraphicsViewer::updateTriangleColors(uint layerNumber)
{
    std::lock_guard<std::mutex> lock(*m_pTrianglesLock);
    for (auto layer : m_graphicsLayers)
    {
        if (layer->getNumber() == layerNumber)
        {
            layer->updateTriangleColors();
            break;
        }
    }
}

void VGraphicsViewer::updateNodeColors(uint layerNumber)
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    for (auto layer : m_graphicsLayers)
    {
        if (layer->getNumber() == layerNumber)
        {
            layer->updateNodeColors();
            break;
        }
    }
}

void VGraphicsViewer::updateVisibility()
{
    for (auto layer : m_graphicsLayers)
        layer->updateVisibility();
}

void VGraphicsViewer::updateVisibility(uint layerNumber)
{
    m_graphicsLayers.at(layerNumber)->updateVisibility();
}

void VGraphicsViewer::updatePosition(uint layerNumber)
{
    m_graphicsLayers.at(layerNumber)->updatePosition();
}

void VGraphicsViewer::updatePositions()
{
    for (auto layer : m_graphicsLayers)
        layer->updatePosition();
}

void VGraphicsViewer::clearNodes() 
{
    std::lock_guard<std::recursive_mutex> locker(m_graphMutex);
    for (auto layer : m_graphicsLayers)
        layer->clearNodes();
    clearSelectedIds();
}

void VGraphicsViewer::clearTriangles() 
{
    std::lock_guard<std::recursive_mutex> locker(m_graphMutex);
    for (auto layer : m_graphicsLayers)
        layer->clearTriangles();
}

void VGraphicsViewer::clearAll() 
{
    std::lock_guard<std::recursive_mutex> locker(m_graphMutex);
    setDragMode(false);
    setSelectionMode(false);
    m_pFigureRoot->removeAllChildren();
    m_graphicsLayers.clear();
    clearSelectedIds();
    clearDraggedNodes();
    clearInfo();
}

void VGraphicsViewer::clearSelectedIds()
{
    m_pSelectedNodesIds.reset(new std::vector<uint>);
}

void VGraphicsViewer::clearDraggedNodes()
{
    m_pTransformedNodesCoords.reset(new std::vector<std::pair<uint, QVector3D> >);
}

void VGraphicsViewer::doRender() 
{
    render();
    m_renderSuccessNotifier.notifyOne();
}

void VGraphicsViewer::displayInfo()
{
    const int PRECISION=7;
    VSimulationInfo inf = m_pSimulator->getSimulationInfo();
    if (inf.iteration > 0)
    {
        m_pSimTimeLabel->setText(QString::number(inf.simTime, 'g', PRECISION));
        m_pRealTimeLabel->setText(QString::number(inf.realTime, 'g', PRECISION));
        m_pRealtimeFactorLabel->setText(QString::number(inf.realtimeFactor, 'g', PRECISION));
        m_pFilledPercentLabel->setText(QString::number(inf.filledPercent, 'g', PRECISION));
        m_pAveragePressureLabel->setText(QString::number(inf.averagePressure, 'g', PRECISION));
        m_pIterationLabel->setText(QString::number(inf.iteration));
    }
    else
        clearInfo();
}

void VGraphicsViewer::clearInfo()
{
    m_pSimTimeLabel->clear();
    m_pRealTimeLabel->clear();
    m_pRealtimeFactorLabel->clear();
    m_pFilledPercentLabel->clear();
    m_pAveragePressureLabel->clear();
    m_pIterationLabel->clear();
}

void VGraphicsViewer::process() 
{
    while(true)
    {
        m_pSimulator->waitForNewData();
        m_renderSuccessNotifier.wait();
        if (!m_renderStopFlag.load())
        {
            {
                std::lock_guard<std::recursive_mutex> locker(m_graphMutex);
                setAutoRedraw(false);
                updateTriangleColors();
                setAutoRedraw(true);
            }
            emit askForRender();
            emit askForDisplayingInfo();
        }
        else
            break;
    }
}

void VGraphicsViewer::stopRender() 
{
    m_renderStopFlag.store(true);
    m_pSimulator->cancelWaitingForNewData();
    m_renderSuccessNotifier.notifyOne();
}

void VGraphicsViewer::viewFromAbove()
{
    stopAnimating();
    SoCamera * pCam = getCamera();
    pCam->position.setValue(SbVec3f(0, 0, 1));
    pCam->orientation.setValue(SbVec3f(0, 0, 1), 0);
    pCam->viewAll( m_pFigureRoot, getViewportRegion() );
}

void VGraphicsViewer::viewFromRight()
{
    stopAnimating();
    SoCamera * pCam = getCamera();
    pCam->position.setValue(SbVec3f(1, 0, 0));
    pCam->orientation.setValue(SbVec3f(1, 1, 1), 2 * static_cast<float>(M_PI) / 3);
    pCam->viewAll( m_pFigureRoot, getViewportRegion() );
}

void VGraphicsViewer::showInjectionPoint()
{
    for (auto &layer : m_graphicsLayers)
        layer->showInjectionPoint();
}
void VGraphicsViewer::showVacuumPoint()
{
    for (auto &layer : m_graphicsLayers)
        layer->showVacuumPoint();
}

void VGraphicsViewer::selectionModeSwitch(bool on)
{
    if (on)
    {
        if (isDragOn())
        {
            m_pSelection->deselectAll();
            setDragMode(false);
        }
        m_pSelectionButton->setVisible(true);
        m_pSelection->policy = SoSelection::SHIFT;
        m_pSelection->lassoType = SoExtSelection::LASSO;
        setViewing(false);
        m_interactionMode = SELECT;
    }
    else if (isSelectionOn())
    {
        {
            std::lock_guard<std::recursive_mutex> locker(m_graphMutex);
            m_pRoot->removeChild(m_pSelection);
            if (m_pOldSelection != nullptr)
                m_pOldSelection->unref();
            m_pOldSelection = m_pSelection;
            m_pSelection = new SoExtSelection;
            m_pSelection->ref();
            initSelection();
            m_pRoot->addChild(m_pSelection);
            m_interactionMode = PICK;
        }
        if (m_pSelectedNodesIds->size() > 0)
        {
            updateNodeColors();
            updateTriangleColors();
            clearSelectedIds();
        }
    }
}

void VGraphicsViewer::dragModeSwitch(bool on)
{
    if (on)
    {
        if (isSelectionOn())
            setSelectionMode(false);
        m_pDragButton->setVisible(true);
        setViewing(false);
        m_interactionMode = DRAG;
    }
    else if (isDragOn())
    {
        m_dragCanceled = true;
        m_pSelection->deselectAll();
        m_interactionMode = PICK;
        emit canceledDrag();
    }
}

void VGraphicsViewer::setSelectionMode(bool on)
{
    m_pSelectionButton->setChecked(on);
}

void VGraphicsViewer::setDragMode(bool on)
{
    m_pDragButton->setChecked(on);
}

void VGraphicsViewer::enableSelection(bool enable)
{
    m_pSelectionButton->setVisible(enable);
    if (!enable)
    {
        if (isSelectionOn())
            setSelectionMode(false);
    }
    emit selectionEnabled(enable);
}

void VGraphicsViewer::enableDrag(bool enable)
{
    m_pDragButton->setVisible(enable);
    if (!enable)
    {
        if (isDragOn())
        {
            setDragMode(false);
        }
        clearDraggedNodes();
    }
    emit dragEnabled(enable);
}

const VGraphicsViewer::const_uint_vect_ptr &VGraphicsViewer::getSelectedNodesIds() const
{
    return m_pSelectedNodesIds;
}

const VGraphicsViewer::const_pos_vect_ptr &VGraphicsViewer::getTransformedNodesCoords() const
{
    return m_pTransformedNodesCoords;
}

uint VGraphicsViewer::getTransformedLayerNumber() const
{
    return m_transformedLayerNumber;
}

bool VGraphicsViewer::isPickOn() const
{
    return (m_interactionMode == PICK);
}

bool VGraphicsViewer::isDragOn() const
{
    return (m_interactionMode == DRAG);
}

bool VGraphicsViewer::isSelectionOn() const
{
    return (m_interactionMode == SELECT);
}

VGraphicsViewer::VInteractionMode VGraphicsViewer::getInteractionMode() const
{
    return m_interactionMode;
}

void VGraphicsViewer::event_cb(void * userdata, SoEventCallback * node)
{
    const float WHEEL_STEP = 0.1f;
    VGraphicsViewer* viewer = static_cast<VGraphicsViewer*>(userdata);
    const SoEvent * event = node->getEvent();

    if (SO_MOUSE_PRESS_EVENT(event, BUTTON1))
    {
        if (!viewer->isPickOn())
            return;
        SoRayPickAction rp( viewer->getViewportRegion() );
        rp.setPoint(event->getPosition());
        rp.apply(viewer->getSceneGraph());
        SoPickedPoint * pickedPoint = rp.getPickedPoint();
        if (pickedPoint != nullptr)
        {
            float x, y, z;
            pickedPoint->getPoint().getValue(x, y, z);
            QVector3D point(x, y, z);
            #ifdef DEBUG_MODE
                qInfo() << "Selected point:" << point.x() << point.y() << point.z();
            #endif
            emit viewer->gotPoint(point);
        }
    }
    else if (SO_MOUSE_PRESS_EVENT(event, BUTTON4))
    {
        viewer->rightWheelMotion(viewer->rightWheelVal - WHEEL_STEP);
    }
    else if (SO_MOUSE_PRESS_EVENT(event, BUTTON5))
    {
        viewer->rightWheelMotion(viewer->rightWheelVal + WHEEL_STEP);
    }
}

void VGraphicsViewer::selection_finish_cb(void * userdata, SoSelection * sel)
{
    VGraphicsViewer* viewer = static_cast<VGraphicsViewer*>(userdata);
    if (!viewer->isSelectionOn())
        return;

    viewer->updateNodeColors();
    viewer->updateTriangleColors();

    std::vector<uint> *selectedNodesVector = new std::vector<uint>;

    const SoPathList* pListSelected = sel->getList();
    selectedNodesVector->reserve(pListSelected->getLength());
    for(int i = 0; i < pListSelected->getLength(); ++i)
    {
        SoPath* path = (*pListSelected)[i];
        for (int j = path->getLength() - 1 ; j >= 0 ; --j)
        {
            VGraphicsElement * pElement = dynamic_cast<VGraphicsElement *>(path->getNode(j));
            if (pElement != nullptr && pElement->isVisible())
            {
                pElement->markAsSelected();
                VGraphicsNode * pNode = dynamic_cast<VGraphicsNode *>(pElement);
                if(pNode != nullptr)
                    selectedNodesVector->push_back(pNode->getSimId());
                break;
            }
        }
    }
    sel->touch();
    sel->deselectAll();
    viewer->m_pSelectedNodesIds.reset(selectedNodesVector);
    if (viewer->m_pSelectedNodesIds->size() > 0)
    {
        emit viewer->gotNodesSelection(viewer->m_pSelectedNodesIds);
    }
}

void VGraphicsViewer::selection_cb(void * userdata, SoPath *selectionPath)
{
    VGraphicsViewer* viewer = static_cast<VGraphicsViewer*>(userdata);

    if (viewer->isDragOn())
    {
        viewer->m_dragCanceled = false;
		if (viewer->m_pTransformBox != nullptr)
		{
			viewer->m_pTransformBox->unref();
			viewer->m_pTransformBox = nullptr;
		}
        viewer->m_pTransformBox = new SoCenterballManip;
        viewer->m_pTransformBox->ref();
    }
    else
        return;

    SoPath *xformPath = createTransformPath(selectionPath);
    if (xformPath == nullptr)
        return;
    xformPath->ref();

    viewer->m_pSelectedPath = xformPath;
    viewer->m_pTransformBox->replaceNode(xformPath);
}

void VGraphicsViewer::deselection_cb(void * userdata, SoPath * deselectionPath)
{
    VGraphicsViewer* viewer = static_cast<VGraphicsViewer*>(userdata);
	if (viewer->m_pSelectedPath != nullptr && viewer->m_pTransformBox != nullptr)
    {
        viewer->m_pTransformBox->replaceManip(viewer->m_pSelectedPath, nullptr);
        viewer->m_pSelectedPath->unref();
        viewer->m_pSelectedPath = nullptr;
        viewer->m_pTransformBox->unref();
        viewer->m_pTransformBox = nullptr;
        if (!viewer->m_dragCanceled && deselectionPath != nullptr)
        {
            SoPath * path = deselectionPath->copy();
            for (int j = deselectionPath->getLength() - 1 ; j >= 0 ; --j)
            {
                VGraphicsLayer * pLayer =
                        dynamic_cast<VGraphicsLayer *>(deselectionPath->getNode(j));
                if (pLayer != nullptr)
                {
                    viewer->m_pTransformedNodesCoords = pLayer->getNodesCoords(
                                viewer->getViewportRegion(), path);
                    viewer->m_transformedLayerNumber = pLayer->getNumber();
                    path->unref();
                    emit viewer->gotTransformation();
                    break;
                }
                path->pop();
            }
            path->unref();
        }
    }
}

inline SoPath * VGraphicsViewer::createTransformPath(SoPath *inputPath)
{
    SoPath * path = inputPath->copy();
    for (int j = inputPath->getLength() - 1 ; j >= 0 ; --j)
    {
        VGraphicsLayer * pLayer = dynamic_cast<VGraphicsLayer *>(inputPath->getNode(j));
        if (pLayer != nullptr && pLayer->isVisible())
        {
            path->append(pLayer->getTransformId());
            return path;
        }
        path->pop();
    }
    path->unref();
    return nullptr;
}

void VGraphicsViewer::leftWheelPressed(void) { leftWheelStart(); }
void VGraphicsViewer::leftWheelChanged(float v) { leftWheelMotion(v); }
void VGraphicsViewer::leftWheelReleased(void) { leftWheelFinish(); }

void VGraphicsViewer::bottomWheelPressed(void) { bottomWheelStart(); }
void VGraphicsViewer::bottomWheelChanged(float v) { bottomWheelMotion(v); }
void VGraphicsViewer::bottomWheelReleased(void) { bottomWheelFinish(); }
