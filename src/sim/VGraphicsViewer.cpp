/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/nodes/SoBaseColor.h>

#include <Inventor/nodes/SoText3.h> //TODO remove
#include "VGraphicsViewer.h"
/**
 * VGraphicsViewer implementation
 */


/**
 * @param parent
 * @param simulator
 */
VGraphicsViewer::VGraphicsViewer(QWidget *parent, VSimulator::ptr simulator) :
    SoQtExaminerViewer (parent),
    m_pSimulator(simulator),
    m_pRenderWaiter(new VRenderWaiter(this)),
    m_pRenderWaiterThread(new QThread)
{
    initGraph();
    m_pRenderWaiter->moveToThread(m_pRenderWaiterThread);
    connect(m_pRenderWaiter, SIGNAL(askForRender()), this, SLOT(doRender()));
    connect(m_pRenderWaiterThread, SIGNAL(started()), m_pRenderWaiter, SLOT(process()));
    connect(m_pRenderWaiter, SIGNAL(finished()), m_pRenderWaiterThread, SLOT(quit()));
    connect(m_pRenderWaiter, SIGNAL(finished()), m_pRenderWaiter, SLOT(deleteLater()));
    connect(m_pRenderWaiterThread, SIGNAL(finished()), m_pRenderWaiterThread, SLOT(deleteLater()));
    m_pRenderWaiterThread->start();
}

VGraphicsViewer::~VGraphicsViewer(){
    m_pRenderWaiter->stop();
}

void VGraphicsViewer::setGraphicsElements(VSimNode::const_vector_ptr nodes,
                                          VSimTriangle::const_vector_ptr triangles) noexcept {
    clearAll();
    createGraphicsElements(&m_graphicsNodes, nodes);
    createGraphicsElements(&m_graphicsTriangles, triangles);
    for (auto node: m_graphicsNodes)
        m_root->addChild(node);
    for (auto triangle: m_graphicsTriangles)
        m_root->addChild(triangle);
}

void VGraphicsViewer::updateTriangleColors() noexcept {
    std::lock_guard<std::mutex> lock(*m_pTrianglesLock);
    for (auto &triangle : m_graphicsTriangles)
        triangle->updateColor();
}

void VGraphicsViewer::clearNodes() noexcept
{
    for (auto node: m_graphicsNodes)
        m_root->removeChild(node);
    m_graphicsNodes.clear();
}

void VGraphicsViewer::clearTriangles() noexcept
{
    for (auto triangle: m_graphicsTriangles)
        m_root->removeChild(triangle);
    m_graphicsTriangles.clear();
}

void VGraphicsViewer::clearAll() noexcept
{
    m_root->removeAllChildren();
    m_graphicsNodes.clear();
    m_graphicsTriangles.clear();
}

void VGraphicsViewer::initGraph() noexcept
{
    //TODO!!!
    // Create a "scene graph"
    m_root = new SoSeparator;
    //root->ref();
    // Set the RGB color. Yellow in this case
    SoBaseColor *color = new SoBaseColor;
    color->rgb = SbColor(1, 1, 0);
    m_root->addChild(color);
    // Create a text
    SoText3 *text3D = new SoText3();
    text3D->string.setValue("Hello SoQt");
    m_root->addChild(text3D);
    setSceneGraph(m_root);
}

void VGraphicsViewer::doRender() noexcept
{
    setAutoRedraw(false);
    updateTriangleColors();
    render();
    setAutoRedraw(true);
    m_renderSuccessLock.unlock();
}

template<typename T1, typename T2>
inline void VGraphicsViewer::createGraphicsElements(std::vector<T1 *>* gaphics,
                                                    std::shared_ptr<const std::vector< std::shared_ptr<T2> > > sim) noexcept
{
    gaphics->clear();
    gaphics->reserve(sim->size());
    for (auto &simElem : *sim)
    {
        if (simElem->isVisible())
            gaphics->push_back(new T1(simElem));
    }
}

/**
  * VRenderWaiter methods are described below
  */

/**
 * @brief VRenderWaiter::VRenderWaiter
 * @param graphicsViewer
 */
VRenderWaiter::VRenderWaiter(VGraphicsViewer *graphicsViewer):
    m_pGraphicsViewer(graphicsViewer)
{
    m_stopFlag.store(false);
}

void VRenderWaiter::process() noexcept
{
    while(true)
    {
        m_pGraphicsViewer->m_pSimulator->waitForNewData();
        m_pGraphicsViewer->m_renderSuccessLock.lock();
        if (!m_stopFlag.load())
            emit askForRender();
        else
            break;
    }
    emit finished();
}

void VRenderWaiter::stop() noexcept
{
    m_stopFlag.store(true);
    m_pGraphicsViewer->m_pSimulator->cancelWaitingForNewData();
    m_pGraphicsViewer->m_renderSuccessLock.unlock();
}
