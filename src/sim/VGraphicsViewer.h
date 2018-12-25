/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VGRAPHICSVIEWER_H
#define _VGRAPHICSVIEWER_H

#include <vector>
#include <memory>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <QPushButton>

#include "VGraphicsNode.h"
#include "VGraphicsTriangle.h"
#include "VSimulator.h"

class SoShapeHints;
class SoPerspectiveCamera;
class SoEventCallback;
class SoExtSelection;
class QWidget;
class QLabel;

class VGraphicsViewer: public VSimulationClass, public SoQtExaminerViewer
{
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

    void updateColors() ;
    void updateNodeColors() ;
    void updateTriangleColors() ;

    void enableSelection(bool enable);

public slots:
    void doRender() ;
    void displayInfo() ;
    void clearInfo() ;

private:
    static const QString LEFT_WHEEL_CAPTION;
    static const QString RIGHT_WHEEL_CAPTION;
    static const QString BOTTOM_WHEEL_CAPTION;
    static const QString REAL_TIME_LABEL_CAPTION;
    static const QString SIM_TIME_LABEL_CAPTION;
    static const QString REALTIME_FACTOR_LABEL_CAPTION;
    static const QString ITERATION_LABEL_CAPTION;
    static const QString FILLED_PERCENT_LABEL_CAPTION;
    static const QString AVERAGE_PRESSURE_LABEL_CAPTION;

    static const int ICON_SIZE = 24;

    void initSelection();

    void stopRender() ;
    void process() ;

    template<typename T1, typename T2>
    inline void createGraphicsElements(std::vector<T1 *>* gaphics,
                                       const std::shared_ptr<const std::vector< std::shared_ptr<T2> > > &sim) ;

    static void event_cb(void * userdata, SoEventCallback * node);
    static void selection_finish_cb(void * userdata, SoSelection * sel);

    std::vector<VGraphicsNode*> m_graphicsNodes;
    std::vector<VGraphicsTriangle*> m_graphicsTriangles;

    VSimulator::ptr m_pSimulator;

    QWidget*            m_pBaseWidget;
    QLabel*             m_pRealTimeLabel;
    QLabel*             m_pSimTimeLabel;
    QLabel*             m_pRealtimeFactorLabel;
    QLabel*             m_pIterationLabel;
    QLabel*             m_pFilledPercentLabel;
    QLabel*             m_pAveragePressureLabel;
    QPushButton*        m_pXYButton;
    QPushButton*        m_pSelectionButton;

    SoSeparator*        m_pRoot;
    SoSeparator*        m_pFigureRoot;
    SoShapeHints*       m_pShapeHints;
    SoPerspectiveCamera* m_pCam;
    SoExtSelection*     m_pSelection;


    std::unique_ptr<std::thread> m_pRenderWaiterThread;
    mutable VNotify m_renderSuccessNotifier;
    std::atomic<bool> m_renderStopFlag;
    mutable std::mutex m_graphMutex;

private slots:
    /**
     * Slot that should be called when the left rotary wheel is pressed, to get ready for a movement.
     */
    void leftWheelPressed(void);

    /**
     * Slot that should be called when the left rotary wheel is changed, to keep track with the current value.
     * @param v New value of the rotary wheel control
     */
    void leftWheelChanged(float v);

    /**
     * Slot that should be called when the left wheel is released, to stop moving the scene graph with the mouse movement.
     */
    void leftWheelReleased(void);

    /**
     * Slot that should be called when the bottom rotary wheel is pressed, to get ready for a movement.
     */
    void bottomWheelPressed(void);

    /**
     * Slot that should be called when the bottom rotary wheel is changed, to keep track with the current value.
     * @param v New value of the rotary wheel control
     */
    void bottomWheelChanged(float v);

    /**
     * Slot that should be called when the bottom wheel is released, to stop moving the scene graph with the mouse movement.
     */
    void bottomWheelReleased(void);

    void setSelectionMode(bool on) ;

protected:
    QWidget* buildLeftTrim(QWidget * parent);
    QWidget* buildBottomTrim(QWidget * parent);
    void createViewerButtons(QWidget * parent, SbPList * buttonlist);
    void toggleCameraType(void);
signals:
    void askForRender();
    void askForDisplayingInfo();
    void gotPoint(const QVector3D &point);
    void gotNodesSelection(const std::shared_ptr<std::vector<uint> > &p_selectedNodes);
};

#endif //_VGRAPHICSVIEWER_H
