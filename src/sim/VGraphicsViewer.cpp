/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoExtSelection.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPointSet.h>

#include <Inventor/nodes/SoText3.h>
#include "VGraphicsViewer.h"
/**
 * VGraphicsViewer implementation
 */


/**
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

void VGraphicsViewer::updateConfiguration() noexcept {

}

void VGraphicsViewer::updateTriangleColors() noexcept {

}

void VGraphicsViewer::clearNodes() noexcept {

}

void VGraphicsViewer::clearTriangles() noexcept {

}

void VGraphicsViewer::clearAll() noexcept {

}

void VGraphicsViewer::initGraph() noexcept
{
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
    /*
    m_fViewerIsEmpty = true;
    setBackgroundColor(SbColor(0.0, 0.0, 0.0));

    m_root = new SoSeparator();
    m_baseColor = new SoBaseColor;
    m_baseColor->rgb = SbColor(1.0f, 0.0f, 0.0f); //grey
    m_nodeMaterial = new SoMaterial();
    m_nodeMaterial->transparency.setValue(0.0f);
    m_root->addChild(m_nodeMaterial);
    m_root->addChild(m_baseColor);
    initSelection();
    setFeedbackVisibility(true);

    //set up root nodes for displaying shapes
    m_shapeRoot = new SoSeparator;
    m_shapeDrawStyle = new SoDrawStyle();
    m_shapeDrawStyle->style = SoDrawStyle::FILLED;
    m_shapeRoot->addChild(m_shapeDrawStyle);
    m_root->addChild(m_shapeRoot);
    // Shape hints tells the ordering of polygons.
    // This ensures double-sided lighting.
    SoShapeHints *shapeHints = new SoShapeHints;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    m_shapeRoot->addChild(shapeHints);

    if(m_fViewerIsEmpty)
    {
        // allow red lines for selection
        SoSeparator * overlaylines = make_line_sg();
        m_absRoot->addChild(overlaylines);
    }
    */
}

/*
void VGraphicsViewer::initSelection() noexcept
{
    m_pSelection = new SoExtSelection;
    m_pSelection->lassoPolicy = SoExtSelection::PART;
    m_pSelection->policy = SoSelection::SHIFT;
    m_pSelection->lassoType = SoExtSelection::LASSO;
    m_pSelection->lassoMode = SoExtSelection::ALL_SHAPES;

    m_pSelection->setLassoColor(SbColor(0.9f,0.7f,0.2f));
    m_pSelection->setLassoWidth(3);
    m_pSelection->setOverlayLassoPattern(0xf0f0);
    m_pSelection->animateOverlayLasso(FALSE);
    //m_pSelection->addStartCallback(start_callback);
    //m_pSelection->addFinishCallback(finish_callback);
    //m_pSelection->setTriangleFilterCallback(triangleCB, NULL);
    //m_pSelection->setLineSegmentFilterCallback(lineCB, NULL);
    //m_pSelection->setPointFilterCallback(pointCB, NULL);


    SoEventCallback * cb = new SoEventCallback;
    //cb->addEventCallback(SoKeyboardEvent::getClassTypeId(), event_cb, NULL);
    //m_root->insertChild(cb, 0);

    //m_pSelection->addChild(m_root);

    if(m_absRoot == NULL)
    {
        m_absRoot = new SoSeparator;
        m_absRoot->ref();
        SoPerspectiveCamera* cam = new SoPerspectiveCamera;
        cam->position.setValue(100,0,0);
        m_absRoot->addChild(cam);
        setCamera(cam);
        setSceneGraph(m_absRoot);
    }
    //m_absRoot->addChild(m_pSelection);
    setTransparencyType(SoGLRenderAction::SORTED_OBJECT_BLEND);
    setGLRenderAction(new SoBoxHighlightRenderAction(SbViewportRegion()));
}
*/
/**
 * Draws a line to display the rubber band.
 */
/*
SoSeparator* VGraphicsViewer::make_line_sg() noexcept
{
    SoSeparator * sep = new SoSeparator;
    SoLightModel * lm = new SoLightModel;
    lm->model = SoLightModel::BASE_COLOR;
    sep->addChild(lm);
    SoBaseColor * col = new SoBaseColor;
    col->rgb = SbColor(1.0f, 0.0f, 0.0f);
    sep->addChild(col);
    SoDrawStyle * ds = new SoDrawStyle;
    ds->lineWidth = 2;
    ds->pointSize = 4;
    sep->addChild(ds);

    SoSeparator *linesep = new SoSeparator;
    sep->addChild(linesep);
    m_coords = new SoCoordinate3;
    m_lineset = new SoIndexedLineSet;
    linesep->addChild(m_coords);
    linesep->addChild(m_lineset);
    SoSeparator *pointsep = new SoSeparator;
    sep->addChild(pointsep);
    m_pointcoords = new SoCoordinate3;
    m_pointcoords->point.setNum(0);
    m_pointset = new SoPointSet;
    pointsep->addChild(m_pointcoords);
    pointsep->addChild(m_pointset);
    return sep;
}
*/

void VGraphicsViewer::doRender() noexcept
{
    setAutoRedraw(false);
    updateTriangleColors();
    render();
    setAutoRedraw(true);
    m_renderSuccessLock.unlock();
}

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
