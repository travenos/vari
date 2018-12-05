/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoShapeHints.h>
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
    SoShapeHints *shapeHints = new SoShapeHints;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    m_pRoot->addChild(shapeHints);
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
                                          const VSimTriangle::const_vector_ptr &triangles) noexcept {
    clearAll();
    createGraphicsElements(&m_graphicsNodes, nodes);
    createGraphicsElements(&m_graphicsTriangles, triangles);
    for (auto node: m_graphicsNodes)
        m_pFigureRoot->addChild(node);
    for (auto triangle: m_graphicsTriangles)
        m_pFigureRoot->addChild(triangle);
}

void VGraphicsViewer::updateTriangleColors() noexcept {
    std::lock_guard<std::mutex> lock(*m_pTrianglesLock);
    for (auto &triangle : m_graphicsTriangles)
        triangle->updateColor();
}

void VGraphicsViewer::clearNodes() noexcept
{
    for (auto node: m_graphicsNodes)
        m_pFigureRoot->removeChild(node);
    m_graphicsNodes.clear();
}

void VGraphicsViewer::clearTriangles() noexcept
{
    for (auto triangle: m_graphicsTriangles)
        m_pFigureRoot->removeChild(triangle);
    m_graphicsTriangles.clear();
}

void VGraphicsViewer::clearAll() noexcept
{
    m_pFigureRoot->removeAllChildren();
    m_graphicsNodes.clear();
    m_graphicsTriangles.clear();
}

void VGraphicsViewer::doRender() noexcept
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
                                                    const std::shared_ptr<const std::vector< std::shared_ptr<T2> > > &sim) noexcept
{
    gaphics->clear();
    gaphics->reserve(sim->size());
    for (auto &simElem : *sim)
    {
        if (simElem->isVisible())
            gaphics->push_back(new T1(simElem));
    }
}

void VGraphicsViewer::process() noexcept
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

void VGraphicsViewer::stopRender() noexcept
{
    m_renderStopFlag.store(true);
    m_pSimulator->cancelWaitingForNewData();
    m_renderSuccessLock.unlock();
}
