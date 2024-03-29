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

#include "graphics_elements/VGraphicsLayer.h"
#include "VSimulator.h"

class SoShapeHints;
class SoCamera;
class SoEventCallback;
class SoExtSelection;
class SoTransformManip;
class QWidget;
class QLabel;

class VGraphicsViewer: public VSimulationClass, public SoQtExaminerViewer
{
    Q_OBJECT
public: 
    typedef std::shared_ptr<VGraphicsViewer> ptr;
    typedef std::shared_ptr<const VGraphicsViewer> const_ptr;

    typedef std::shared_ptr<std::vector<uint> > uint_vect_ptr;
    typedef std::shared_ptr<const std::vector<uint> > const_uint_vect_ptr;
    typedef std::shared_ptr<std::vector<std::pair<uint, QVector3D> > > pos_vect_ptr;
    typedef std::shared_ptr<const std::vector<std::pair<uint, QVector3D> > > const_pos_vect_ptr;

    enum VInteractionMode {PICK, DRAG, SELECT};

    VGraphicsViewer(QWidget* parent, const VSimulator::ptr &simulator);
    virtual ~VGraphicsViewer();
    void setGraphicsElements(const std::vector<VLayer::const_ptr> &layers) ;
    void updateVisibility();
    void updateVisibility(uint layerId);
    void clearNodes() ;
    void clearTriangles() ;
    void clearAll() ;
    void clearSelectedIds() ;
    void clearDraggedNodes() ;

    void showInjectionPoint();
    void showVacuumPoint();

    void updateColors() ;
    void updateNodeColors() ;
    void updateTriangleColors() ;
    void updateColors(uint layerId) ;
    void updateNodeColors(uint layerId) ;
    void updateTriangleColors(uint layerId) ;

    void updatePosition(uint layerId) ;
    void updatePositions() ;

    void enableSelection(bool enable);
    void enableDrag(bool enable);

    const const_uint_vect_ptr &getSelectedNodesIds() const;
    const const_pos_vect_ptr &getTransformedNodesCoords() const;
    uint getTransformedLayerId() const;

    bool isPickOn() const;
    bool isDragOn() const;
    bool isSelectionOn() const;
    VInteractionMode getInteractionMode() const;

    void setSelectionMode(bool on) ;
    void setDragMode(bool on) ;

    void setCubeSide(float side);
    float getCubeSide() const;

    bool isCameraOrthographic() const;
    void setCameraOrthographic(bool on);

    void enableAdditionalControls(bool enable);
    bool areAdditionalControlsEnabled() const;

public slots:
    void doRender() ;
    void displayInfo() ;
    void clearInfo() ;

    void viewFromAbove() ;
    void viewFromRight() ;

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

    static const QString SELECTION_MODE_TOOLTIP;
    static const QString VIEW_MOVEMENT_MODE_TOOLTIP;
    static const QString TO_SAVED_VIEWPOINT_TOOLTIP;
    static const QString SAVE_VIEWPOINT_TOOLTIP;
    static const QString FIT_TO_SCREEN_TOOLTIP;
    static const QString ZOOM_MODE_TOOLTIP;
    static const QString PROJECTION_TYPE_TOOLTIP;
    static const QString XY_TOOLTIP;
    static const QString YZ_TOOLTIP;
    static const QString DRAG_TOOLTIP;
    static const QString CUT_SELECTION_TOOLTIP;

    static const int ICON_SIZE;

    void initSelection();

    void stopRender() ;
    void process() ;

    void enableDragPrivate(bool enable);

    static void event_cb(void * userdata, SoEventCallback * node);
    /**
     * Callback static function for the lasso selection mode. Should be called when the select action finishes for post processing.
     * @param userdata pointer to master VGraphicsViewer object
     * @param sel Pointer to the used selection object.
     */
    static void selection_finish_cb(void * userdata, SoSelection * sel);
    static void selection_cb(void * userdata, SoPath *selectionPath);
    static void deselection_cb(void * userdata, SoPath *deselectioPath);

    static inline SoPath * createTransformPath(SoPath *inputPath);

    std::vector<VGraphicsLayer*> m_graphicsLayers;

    VSimulator::ptr m_pSimulator;

    QWidget*            m_pBaseWidget;
    QLabel*             m_pRealTimeLabel;
    QLabel*             m_pSimTimeLabel;
    QLabel*             m_pRealtimeFactorLabel;
    QLabel*             m_pIterationLabel;
    QLabel*             m_pFilledPercentLabel;
    QLabel*             m_pAveragePressureLabel;
    QPushButton*        m_pXYButton;
    QPushButton*        m_pYZButton;
    QPushButton*        m_pSelectionButton;
    QPushButton*        m_pDragButton;

    SoSeparator*        m_pRoot;
    SoSeparator*        m_pFigureRoot;
    SoShapeHints*       m_pShapeHints;
    SoCamera*           m_pCam;
    SoExtSelection*     m_pSelection;
    SoExtSelection*     m_pOldSelection;

    SoTransformManip*   m_pTransformBox;
    SoPath*             m_pSelectedPath;

    std::unique_ptr<std::thread> m_pRenderWaiterThread;
    mutable VNotify m_renderSuccessNotifier;
    std::atomic<bool> m_renderStopFlag;
    VInteractionMode m_interactionMode;
    bool m_dragCanceled;
    mutable std::recursive_mutex m_graphMutex;

    const_uint_vect_ptr m_pSelectedNodesIds;
    const_pos_vect_ptr m_pTransformedNodesCoords;
    uint m_transformedLayerId;

    float m_cubeSide;

    bool m_dragEnabled;
    bool m_additionalGraphicsControlsEnabled;

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

    void selectionModeSwitch(bool on) ;
    void dragModeSwitch(bool on) ;

protected:
    QWidget* buildLeftTrim(QWidget * parent);
    QWidget* buildBottomTrim(QWidget * parent);
    void createViewerButtons(QWidget * parent, SbPList * buttonlist);
signals:
    void askForRender();
    void askForDisplayingInfo();
    void canceledDrag();
    void interactionEnabled(bool);
    void selectionEnabled(bool);
    void dragEnabled(bool);
    void gotPoint(const QVector3D &point);
    void gotNodesSelection(const VGraphicsViewer::const_uint_vect_ptr &p_selectedNodes);
    void gotTransformation();
    void cubeSideChanged(float side);
    void additionalControlsEnabled(bool);
};


#endif //_VGRAPHICSVIEWER_H
