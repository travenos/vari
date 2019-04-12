/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VWINDOWPOLYGON_H
#define _VWINDOWPOLYGON_H

#include <memory>

#include <QMainWindow>

class QCPCurve;
class QCPItemEllipse;
class QCPRange;
class QShortcut;

class VTable;

namespace Ui {
class VWindowPolygon;
}

struct VMouseInteractionInfo
{
    bool mousePressed{false};
    bool plotDragged{false};
    bool vertexSelected{false};
    bool pressedInCorrectPlace{false};
    int selectedIndex{-1};
};

class VWindowPolygon : public QMainWindow
{
    Q_OBJECT

public:
    static const QCPRange X_RANGE;
    static const QCPRange Y_RANGE;
    static const int MIN_POLYGON_SIZE;
    static const double MIN_CHARACTERISTIC_LENGTH;
    static const double DEFAULT_CHARACTERISTIC_LENGTH;
    static const double MIN_CHARACTERISTIC_RATIO;

    static const QString SAVE_FILE_DIALOG_TITLE;
    static const QString FILE_DIALOG_FORMATS;
    static const QString ERROR_TITLE;
    static const QString EXPORT_TO_FILE_ERROR;
    static const QString TOO_SMALL_STEP_ERROR;
    static const QString INTERSECTION_ERROR;
    static const QString TABLE_ERROR;

    static const int POINT_SIZE;
    static const int HIGHLIGHT_POINT_SIZE;

    VWindowPolygon(QWidget *parent = nullptr,
                   std::shared_ptr<const VTable> p_table=nullptr);
    virtual ~VWindowPolygon();

    void reset();
    void resetView();

    void addVertex(double x, double y);
    void removeLast();
    void plot();
    void plotEnclosed();
    void updateButtonsStates();
    void getPolygon(QPolygonF &polygon) const;
    int getPolygonSize() const;
    void exportMeshToFile(const QString &filename) const;

    void loadSavedParameters();
    void saveParameters() const;

    double getStepRatio() const;

    void setTable(const std::shared_ptr<const VTable> & p_table);
    void setUseTable(bool use);

private:
    Ui::VWindowPolygon *ui;
    QCPCurve * m_pPlotCurve;
    QCPCurve * m_pCloseCurve;
    QCPCurve * m_pHighlightCurve;
    QCPCurve * m_pTableCurve;
    QCPItemEllipse *m_pInjectionEllipse;
    QCPItemEllipse *m_pVacuumEllipse;
    QShortcut * m_pUndoShortcut;

    VMouseInteractionInfo m_mouseInfo;

    double m_characteristicLength;

    QVector<double> m_qvT;
    QVector<double> m_qvX;
    QVector<double> m_qvY;

    QString m_lastDir;

    std::shared_ptr<const VTable> m_pTable;
    bool m_useTable;

    void reject();
    void accept();
    void meshExportProcedure();
    void showCharacteristicLength();
    void showRatioError(double ratio);
    void showIntersectionError();
    void showTableError();
    bool newVertexCausesIntersection(double x, double y) const;
    bool removingVertexCausesIntersection(int index) const;
    bool lastLineCausesIntersection() const;
    bool vertexCausesIntersection(int index, double x, double y) const;
    bool vertexIsOkForTable(double x, double y) const;
    QString getVertexString(double x, double y) const;
    void highlight(int index);
    void selectVertex();
    void removeVertex(int index);
    void changeVertex(int index, double x, double y);
    void showCoords(int index);
    void addVertexToList(int index);
    void removeVertexFromList(int index);
    void updateVertexRecord(int index);

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();

    void pl_on_before_report();
    void pl_on_mouse_release(QMouseEvent* event);
    void pl_on_mouse_press(QMouseEvent* event);
    void pl_on_mouse_move(QMouseEvent* event);

    void on_undoButton_clicked();

    void on_clearButton_clicked();

    void on_resetViewButton_clicked();

    void on_exportButton_clicked();

    void on_stepSpinBox_valueChanged(double arg1);

    void on_verticesListWidget_itemSelectionChanged();

    void on_restoreCoordsButton_clicked();

    void on_changeCoordsButton_clicked();

    void on_removeVertexButton_clicked();

    void on_changeXSpinBox_valueChanged(double arg1);

    void on_changeYSpinBox_valueChanged(double arg1);

    void on_addVertexButton_clicked();

    void on_useTableCheckBox_clicked(bool checked);

protected:

    virtual void closeEvent(QCloseEvent *) override;

signals:
    void polygonAvailable(const QPolygonF &polygon, double characteristicLength);
    void creationCanceled();
    void windowClosed();
};

#endif // _VWINDOWPOLYGON_H
