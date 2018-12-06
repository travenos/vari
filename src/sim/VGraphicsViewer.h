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

class VGraphicsViewer: public QObject, public SoQtExaminerViewer, public VSimulationClass {
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
    //TODO remove operator= and copy constructor
    void setGraphicsElements(const VSimNode::const_vector_ptr &nodes,
                             const VSimTriangle::const_vector_ptr &triangles) ;
    void updateTriangleColors() ;
    void clearNodes() ;
    void clearTriangles() ;
    void clearAll() ;
private:
    void stopRender() ;
    void process() ;
    template<typename T1, typename T2>
    inline void createGraphicsElements(std::vector<T1 *>* gaphics,
                                       const std::shared_ptr<const std::vector< std::shared_ptr<T2> > > &sim) ;

    std::vector<VGraphicsNode*> m_graphicsNodes;
    std::vector<VGraphicsTriangle*> m_graphicsTriangles;

    VSimulator::ptr m_pSimulator;

    SoSeparator*        m_pRoot;
    SoSeparator*        m_pFigureRoot;

    std::unique_ptr<std::thread> m_pRenderWaiterThread;
    std::mutex m_renderSuccessLock;
    std::atomic<bool> m_renderStopFlag;

private slots:
    void doRender() ;
signals:
    void askForRender();
};

#endif //_VGRAPHICSVIEWER_H
