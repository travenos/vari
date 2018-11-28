/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSVIEWER_H
#define _VGRAPHICSVIEWER_H

#include <vector>
#include <memory>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <QThread>
#include "VGraphicsNode.h"
#include "VGraphicsTriangle.h"
#include "VSimulator.h"

class VRenderWaiter;

class VGraphicsViewer: public QObject, public SoQtExaminerViewer, public VSimulationClass {
    Q_OBJECT
    friend class VRenderWaiter;
public: 
    typedef std::shared_ptr<VGraphicsViewer> ptr;
    typedef std::shared_ptr<const VGraphicsViewer> const_ptr;
    /**
     * @param parent
     * @param simulator
     */
    VGraphicsViewer(QWidget* parent, VSimulator::ptr simulator);
    virtual ~VGraphicsViewer();
    void setGraphicsElements(VSimNode::const_vector_ptr nodes,
                             VSimTriangle::const_vector_ptr triangles) noexcept;
    void updateTriangleColors() noexcept;
    void clearNodes() noexcept;
    void clearTriangles() noexcept;
    void clearAll() noexcept;
private: 
    void initGraph() noexcept;
    template<typename T1, typename T2>
    inline void createGraphicsElements(std::vector<T1 *>* gaphics,
                                       std::shared_ptr<const std::vector< std::shared_ptr<T2> > > sim) noexcept;

    std::vector<VGraphicsNode*> m_graphicsNodes;
    std::vector<VGraphicsTriangle*> m_graphicsTriangles;

    VSimulator::ptr m_pSimulator;

    SoSeparator*        m_root;

    VRenderWaiter* m_pRenderWaiter;
    QThread* m_pRenderWaiterThread;
    std::mutex m_renderSuccessLock;

private slots:
    void doRender() noexcept;
};

class VRenderWaiter : public QObject
{
    Q_OBJECT
    friend class VGraphicsViewer;
private:
    VRenderWaiter(VGraphicsViewer* graphicsViewer);
    VRenderWaiter() = delete;
    VRenderWaiter(const VRenderWaiter&) = delete;
    VRenderWaiter& operator =(const VRenderWaiter&) = delete;

    VGraphicsViewer *const m_pGraphicsViewer;
    std::atomic<bool> m_stopFlag;
public slots:
    void process() noexcept;
    void stop() noexcept;
signals:
    void askForRender();
    void finished();
};

#endif //_VGRAPHICSVIEWER_H
