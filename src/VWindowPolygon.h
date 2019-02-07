/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VWINDOWPOLYGON_H
#define _VWINDOWPOLYGON_H

#include <QMainWindow>

class QCPCurve;
class QCPRange;

namespace Ui {
class VWindowPolygon;
}

class VWindowPolygon : public QMainWindow
{
    Q_OBJECT

public:
    static const QCPRange X_RANGE;
    static const QCPRange Y_RANGE;
    static const int MIN_POLYGON_SIZE;

    static const QString SAVE_FILE_DIALOG_TITLE;
    static const QString FILE_DIALOG_FORMATS;
    static const QString ERROR_TITLE;
    static const QString EXPORT_TO_FILE_ERROR;

    VWindowPolygon(QWidget *parent = nullptr);
    virtual ~VWindowPolygon();

    void reset();
    void resetView();

    void addPoint(double x, double y);
    void removeLast();
    void plot();
    void plotEnclosed();
    void updateButtonsStates();
    void getPolygon(QPolygonF &polygon) const;
    int getPolygonSize() const;
    void exportMeshToFile(const QString &filename) const;

    void loadSavedParameters();
    void saveParameters() const;

private:
    Ui::VWindowPolygon *ui;
    QCPCurve * m_pPlotCurve;
    QCPCurve * m_pCloseCurve;

    bool m_mousePressed;
    bool m_plotDragged;

    QVector<double> m_qvT;
    QVector<double> m_qvX;
    QVector<double> m_qvY;

    QString m_lastDir;

    void reject();
    void accept();
    void meshExportProcedure();

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

    void on_encloseButton_clicked();

    void on_exportButton_clicked();

protected:

    virtual void closeEvent(QCloseEvent *) override;

signals:
    void polygonAvailable(const QPolygonF &polygon);
    void creationCanceled();
    void windowClosed();
};

#endif // _VWINDOWPOLYGON_H
