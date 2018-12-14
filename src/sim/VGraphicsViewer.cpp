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
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/Qt/widgets/SoQtThumbWheel.h>
#include <functional>

#include "VGraphicsViewer.h"
/**
 * VGraphicsViewer implementation
 */

const QString VGraphicsViewer::LEFT_WHEEL_CAPTION("Y");
const QString VGraphicsViewer::RIGHT_WHEEL_CAPTION("Масштаб");
const QString VGraphicsViewer::BOTTOM_WHEEL_CAPTION("X");
const QString VGraphicsViewer::REAL_TIME_LABEL_CAPTION("Время моделирования (с):");
const QString VGraphicsViewer::SIM_TIME_LABEL_CAPTION("Время процесса (с):");
const QString VGraphicsViewer::REALTIME_FACTOR_LABEL_CAPTION("Фактор реального времни:");
const QString VGraphicsViewer::ITERATION_LABEL_CAPTION("Номер итерации:");
const QString VGraphicsViewer::FILLED_PERCENT_LABEL_CAPTION("Готовность эксперимента (%):");
const QString VGraphicsViewer::AVERAGE_PRESSURE_LABEL_CAPTION("Среднее давление (Па):");

/**
 * @param parent
 * @param simulator
 */
VGraphicsViewer::VGraphicsViewer(QWidget *parent, const VSimulator::ptr &simulator) :
    SoQtExaminerViewer (parent, nullptr, true, SoQtFullViewer::BUILD_ALL, SoQtFullViewer::BROWSER, false),
    m_pSimulator(simulator),
    m_pRoot(new SoSeparator),
    m_pFigureRoot(new SoSeparator),
    m_pShapeHints(new SoShapeHints),
    m_pCam(new SoPerspectiveCamera),
    m_renderStopFlag(false)
{
    m_pBaseWidget = buildWidget(getParentWidget());
    setBaseWidget(m_pBaseWidget);

    SoEventCallback * cb = new SoEventCallback;
    cb->addEventCallback(SoMouseButtonEvent::getClassTypeId(), event_cb, this);
    m_pRoot->insertChild(cb, 0);
    m_pShapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    m_pRoot->addChild(m_pShapeHints);
    m_pRoot->addChild(m_pCam);
    m_pRoot->addChild(m_pFigureRoot);
    setSceneGraph(m_pRoot);
    setPopupMenuEnabled(false);

    connect(this, SIGNAL(askForRender()), this, SLOT(doRender()));
    connect(this, SIGNAL(askForDisplayingInfo()), this, SLOT(displayInfo()));
    m_pRenderWaiterThread.reset(new std::thread(std::bind(&VGraphicsViewer::process, this)));
    m_renderSuccessNotifier.notifyOne();
}

VGraphicsViewer::~VGraphicsViewer()
{
    stopRender();
    m_pRenderWaiterThread->join();
    m_pRenderWaiterThread.reset();
    m_pRoot->removeAllChildren();
    setSceneGraph(NULL);
    setCamera(NULL);
}

QWidget* VGraphicsViewer::buildLeftTrim(QWidget * parent)
{
    const int WIDGET_WIDTH = 45;
    const int HORIZONTAL_SPACING = 4;
    QWidget * widget = new QWidget(parent);
    widget->setFixedWidth(WIDGET_WIDTH);

    QGridLayout * layout = new QGridLayout(widget);
    //gl->addWidget(this->buildAppButtons(w), 0, 0);

    SoQtThumbWheel * wheel = new SoQtThumbWheel(SoQtThumbWheel::Vertical, widget);
    wheel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->leftWheel = wheel;
    wheel->setRangeBoundaryHandling(SoQtThumbWheel::ACCUMULATE);
    this->leftWheelVal = wheel->value();

    QObject::connect(wheel, SIGNAL(wheelMoved(float)), this, SLOT(leftWheelChanged(float)));
    QObject::connect(wheel, SIGNAL(wheelPressed()), this, SLOT(leftWheelPressed()));
    QObject::connect(wheel, SIGNAL(wheelReleased()), this, SLOT(leftWheelReleased()));

    QLabel * colorScaleLabel = new QLabel(widget);
    colorScaleLabel->setPixmap(QPixmap(":/img/colormap.png"));

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
    //gl->addWidget(this->buildAppButtons(w), 0, 0);

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

void VGraphicsViewer::setGraphicsElements(const VSimNode::const_vector_ptr &nodes,
                                          const VSimTriangle::const_vector_ptr &triangles)
{
    clearAll();
    createGraphicsElements(&m_graphicsNodes, nodes);
    createGraphicsElements(&m_graphicsTriangles, triangles);
    for (auto node: m_graphicsNodes)
        m_pFigureRoot->addChild(node);
    for (auto triangle: m_graphicsTriangles)
        m_pFigureRoot->addChild(triangle);
}

void VGraphicsViewer::updateColors()
{
    updateTriangleColors();
    updateNodeColors();
}

void VGraphicsViewer::updateTriangleColors()
{
    std::lock_guard<std::mutex> lock(*m_pTrianglesLock);
    for (auto &triangle : m_graphicsTriangles)
        triangle->updateColor();
}

void VGraphicsViewer::updateNodeColors()
{
    std::lock_guard<std::mutex> lock(*m_pNodesLock);
    for (auto &node : m_graphicsNodes)
        node->updateColor();
}

void VGraphicsViewer::updateVisibility()
{
    for (auto &node : m_graphicsNodes)
        node->updateVisibility();
    for (auto &triangle : m_graphicsTriangles)
        triangle->updateVisibility();
}

void VGraphicsViewer::clearNodes() 
{
    for (auto node: m_graphicsNodes)
        m_pFigureRoot->removeChild(node);
    m_graphicsNodes.clear();
}

void VGraphicsViewer::clearTriangles() 
{
    for (auto triangle: m_graphicsTriangles)
        m_pFigureRoot->removeChild(triangle);
    m_graphicsTriangles.clear();
}

void VGraphicsViewer::clearAll() 
{
    m_pFigureRoot->removeAllChildren();
    m_graphicsNodes.clear();
    m_graphicsTriangles.clear();
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

template<typename T1, typename T2>
inline void VGraphicsViewer::createGraphicsElements(std::vector<T1 *>* gaphics,
                                                    const std::shared_ptr<const std::vector< std::shared_ptr<T2> > > &sim) 
{
    gaphics->clear();
    gaphics->reserve(sim->size());
    for (auto &simElem : *sim)
        gaphics->push_back(new T1(simElem));
}

void VGraphicsViewer::process() 
{
    while(true)
    {
        m_pSimulator->waitForNewData();
        m_renderSuccessNotifier.wait();
        if (!m_renderStopFlag.load())
        {
            setAutoRedraw(false);
            updateTriangleColors();
            setAutoRedraw(true);
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
    m_pCam->position.setValue(SbVec3f(0, 0, 1));
    m_pCam->orientation.setValue(SbVec3f(0, 0, 1), 0);
    m_pCam->viewAll( m_pFigureRoot, getViewportRegion() );
}

void VGraphicsViewer::showInjectionPoint()
{
    for (auto &node : m_graphicsNodes)
        node->colorIfInjection();
    for (auto &triangle : m_graphicsTriangles)
        triangle->colorIfInjection();
}
void VGraphicsViewer::showVacuumPoint()
{
    for (auto &node : m_graphicsNodes)
        node->colorIfVacuum();
    for (auto &triangle : m_graphicsTriangles)
        triangle->colorIfVacuum();
}

void VGraphicsViewer::event_cb(void * userdata, SoEventCallback * node)
{
    VGraphicsViewer* viewer = static_cast<VGraphicsViewer*>(userdata);
    const SoEvent * event = node->getEvent();

    if (SO_MOUSE_PRESS_EVENT(event, BUTTON1))
    {
        SoRayPickAction rp( viewer->getViewportRegion() );
        rp.setPoint(event->getPosition());
        rp.apply(viewer->getSceneGraph());
        SoPickedPoint * pickedPoint = rp.getPickedPoint();
        if (pickedPoint != NULL)
        {
            float x, y, z;
            pickedPoint->getPoint().getValue(x, y, z);
            QVector3D point(x, y, z);
            viewer->emitGotPoint(point);
        }
    }
}

void VGraphicsViewer::emitGotPoint(const QVector3D &point)
{
    #ifdef DEBUG_MODE
        qInfo() << "Selected point:" << point.x() << point.y() << point.z();
    #endif
    emit gotPoint(point);
}

void VGraphicsViewer::leftWheelPressed(void) { leftWheelStart(); }
void VGraphicsViewer::leftWheelChanged(float v) { leftWheelMotion(v); }
void VGraphicsViewer::leftWheelReleased(void) { leftWheelFinish(); }

void VGraphicsViewer::bottomWheelPressed(void) { bottomWheelStart(); }
void VGraphicsViewer::bottomWheelChanged(float v) { bottomWheelMotion(v); }
void VGraphicsViewer::bottomWheelReleased(void) { bottomWheelFinish(); }
