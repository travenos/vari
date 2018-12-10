/**
 * Project VARI
 * @author Alexey Barashkov
 */
#ifdef DEBUG_MODE
#include <QDebug>
#endif
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/SoPickedPoint.h>
#include <functional>

#include "VGraphicsViewer.h"
/**
 * VGraphicsViewer implementation
 */


/**
 * @param parent
 * @param simulator
 */
VGraphicsViewer::VGraphicsViewer(QWidget *parent, const VSimulator::ptr &simulator) :
    SoQtExaminerViewer (parent),
    m_pSimulator(simulator),
    m_pRoot(new SoSeparator),
    m_renderStopFlag(false)
{
    SoEventCallback * cb = new SoEventCallback;
    cb->addEventCallback(SoMouseButtonEvent::getClassTypeId(), event_cb, this);
    m_pRoot->insertChild(cb, 0);
    SoShapeHints *shapeHints = new SoShapeHints;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    m_pRoot->addChild(shapeHints);
    m_pCam = new SoPerspectiveCamera;
    m_pRoot->addChild(m_pCam);
    m_pFigureRoot = new SoSeparator;
    m_pRoot->addChild(m_pFigureRoot);
    setSceneGraph(m_pRoot);

    connect(this, SIGNAL(askForRender()), this, SLOT(doRender()));
    m_pRenderWaiterThread.reset(new std::thread(std::bind(&VGraphicsViewer::process, this)));
}

VGraphicsViewer::~VGraphicsViewer(){
    stopRender();
    m_pRenderWaiterThread->join();
    m_pRenderWaiterThread.reset();
    m_pRoot->removeAllChildren();
    setSceneGraph(NULL);
    setCamera(NULL);
}

void VGraphicsViewer::setGraphicsElements(const VSimNode::const_vector_ptr &nodes,
                                          const VSimTriangle::const_vector_ptr &triangles)  {
    clearAll();
    createGraphicsElements(&m_graphicsNodes, nodes);
    createGraphicsElements(&m_graphicsTriangles, triangles);
    for (auto node: m_graphicsNodes)
        m_pFigureRoot->addChild(node);
    for (auto triangle: m_graphicsTriangles)
        m_pFigureRoot->addChild(triangle);
}

void VGraphicsViewer::updateTriangleColors()
{
    std::lock_guard<std::mutex> lock(*m_pTrianglesLock);
    for (auto &triangle : m_graphicsTriangles)
        triangle->updateColor();
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
    setAutoRedraw(false);
    updateTriangleColors();
    render();
    setAutoRedraw(true);
    //TODO print simulation info
    m_renderSuccessLock.unlock();
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
        m_renderSuccessLock.lock();
        if (!m_renderStopFlag.load())
            emit askForRender();
        else
            break;
    }
}

void VGraphicsViewer::stopRender() 
{
    m_renderStopFlag.store(true);
    m_pSimulator->cancelWaitingForNewData();
    m_renderSuccessLock.unlock();
}

void VGraphicsViewer::viewFromAbove()
{
    m_pCam->position.setValue(SbVec3f(0, 0, 1));
    m_pCam->orientation.setValue(SbVec3f(0, 0, 1), 0);
    m_pCam->viewAll( m_pFigureRoot, getViewportRegion() );
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
        qDebug() << "Selected point:" << point.x() << point.y() << point.z();
    #endif
    emit gotPoint(point);
}
