/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VWINDOWPOLYGON_H
#define _VWINDOWPOLYGON_H

#include <QMainWindow>

namespace Ui {
class VWindowPolygon;
}

class VWindowPolygon : public QMainWindow
{
    Q_OBJECT

public:
    VWindowPolygon(QWidget *parent = nullptr);
    virtual ~VWindowPolygon();

private:
    Ui::VWindowPolygon *ui;

    bool m_mousePressed;
    bool m_plotDragged;

    QVector<double> m_qvX;
    QVector<double> m_qvY;

    void reset();
    void reject();
    void accept();

    void resetView();

    void addPoint(double x, double y);
    void removeLast();
    void plot();
    void plotEnclosed();
    void updateButtonsStates();
    void getPolygon(QPolygonF &polygon) const;
    int getPolygonSize() const;

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

    void on_enclosedButton_clicked();

    void on_exportButton_clicked();

protected:

    virtual void closeEvent(QCloseEvent *) override;

signals:
    void polygonAvailable(const QPolygonF &polygon);
    void creationCanceled();
    void windowClosed();
};

#endif // _VWINDOWPOLYGON_H
