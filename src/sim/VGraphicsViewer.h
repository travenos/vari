/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSVIEWER_H
#define _VGRAPHICSVIEWER_H

#include <vector>
#include <memory>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <QObject>
#include "VGraphicsNode.h"
#include "VGraphicsTriangle.h"
#include "VSimulator.h"

class SoPerspectiveCamera;
class SoEventCallback;

class VGraphicsViewer: public QObject, protected SoQtExaminerViewer, public VSimulationClass {
    Q_OBJECT
public: 
    typedef std::shared_ptr<VGraphicsViewer> ptr;
    typedef std::shared_ptr<const VGraphicsViewer> const_ptr;
    /**
     * @param parent
     * @param simulator
     */
    VGraphicsViewer(QWidget* parent, const VSimulator::ptr &simulator);
    virtual ~VGraphicsViewer();
    void setGraphicsElements(const VSimNode::const_vector_ptr &nodes,
                             const VSimTriangle::const_vector_ptr &triangles) ;
    void updateVisibility();
    void clearNodes() ;
    void clearTriangles() ;
    void clearAll() ;
    void viewFromAbove() ;

    void showInjectionPoint();
    void showVacuumPoint();

private:
    void emitGotPoint(const QVector3D &point);

    void stopRender() ;
    void process() ;
    void updateTriangleColors() ;

    template<typename T1, typename T2>
    inline void createGraphicsElements(std::vector<T1 *>* gaphics,
                                       const std::shared_ptr<const std::vector< std::shared_ptr<T2> > > &sim) ;

    static void event_cb(void * userdata, SoEventCallback * node);

    std::vector<VGraphicsNode*> m_graphicsNodes;
    std::vector<VGraphicsTriangle*> m_graphicsTriangles;

    VSimulator::ptr m_pSimulator;

    SoSeparator*        m_pRoot;
    SoSeparator*        m_pFigureRoot;
    SoPerspectiveCamera* m_pCam;

    std::unique_ptr<std::thread> m_pRenderWaiterThread;
    mutable VNotify m_renderSuccessNotifier;
    mutable std::mutex m_viewMutex;
    std::atomic<bool> m_renderStopFlag;

private slots:
    void doRender() ;
signals:
    void askForRender();
    void gotPoint(const QVector3D &point);
};

#endif //_VGRAPHICSVIEWER_H
