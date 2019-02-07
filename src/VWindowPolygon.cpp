/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif

#include <algorithm>

#include "VWindowPolygon.h"
#include "ui_VWindowPolygon.h"

const QCPRange X_RANGE(-0.5, 0.5);
const QCPRange Y_RANGE(-0.5, 0.5);
const int MIN_POLYGON_SIZE = 3;

VWindowPolygon::VWindowPolygon(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VWindowPolygon),
    m_mousePressed(false),
    m_plotDragged(false)
{
    ui->setupUi(this);

    updateButtonsStates();
    resetView();

    ui->plotWidget->setInteraction(QCP::iRangeDrag, true);
    ui->plotWidget->setInteraction(QCP::iRangeZoom, true);
    ui->plotWidget->addGraph();
    ui->plotWidget->graph(0)->setScatterStyle(QCPScatterStyle::ssPlusCircle);
    ui->plotWidget->graph(0)->setLineStyle(QCPGraph::lsLine);
    connect(ui->plotWidget, SIGNAL(beforeReplot()), this, SLOT(pl_on_before_report()));
    connect(ui->plotWidget, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(pl_on_mouse_release(QMouseEvent*)));
    connect(ui->plotWidget, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(pl_on_mouse_press(QMouseEvent*)));
    connect(ui->plotWidget, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(pl_on_mouse_move(QMouseEvent*)));
}

VWindowPolygon::~VWindowPolygon()
{
    delete ui;
    #ifdef DEBUG_MODE
        qInfo() << "VWindowPolygon destroyed";
    #endif
}

void VWindowPolygon::accept()
{
    hide();
    QPolygonF polygon;
    getPolygon(polygon);
    emit polygonAvailable(polygon);
    close();
}

void VWindowPolygon::reset()
{
    m_qvX.clear();
    m_qvY.clear();
    plot();
    updateButtonsStates();
    resetView();
}

void VWindowPolygon::resetView()
{
    ui->plotWidget->xAxis->setRange(X_RANGE);
    ui->plotWidget->yAxis->setRange(Y_RANGE);
    ui->plotWidget->replot();
}

void VWindowPolygon::reject()
{
    hide();
    emit creationCanceled();
    close();
}

void VWindowPolygon::addPoint(double x, double y)
{
    m_qvX.append(x);
    m_qvY.append(y);
    plot();
    updateButtonsStates();
}

void VWindowPolygon::removeLast()
{
    if (m_qvX.size() > 0)
        m_qvX.pop_back();
    if (m_qvY.size() > 0)
        m_qvY.pop_back();
    plot();
    updateButtonsStates();
}

void VWindowPolygon::plot()
{
    ui->plotWidget->graph(0)->setData(m_qvX, m_qvY, true);
    ui->plotWidget->replot();
    ui->plotWidget->update();
}

void VWindowPolygon::plotEnclosed()
{
    QVector<double> coordX, coordY;
    coordX = m_qvX;
    coordY = m_qvY;
    if (coordX.size() > 1)
        coordX.push_back(coordX[0]);
    if (coordY.size() > 1)
        coordY.push_back(coordY[0]);
    ui->plotWidget->graph(0)->setData(coordX, coordY, true);
    ui->plotWidget->replot();
    ui->plotWidget->update();
}

void VWindowPolygon::updateButtonsStates()
{
    bool hasValidPolygon = (getPolygonSize() >= MIN_POLYGON_SIZE);
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(hasValidPolygon);
    ui->exportButton->setEnabled(hasValidPolygon);
    bool hasPoints = (getPolygonSize() > 0);
    ui->undoButton->setEnabled(hasPoints);
    ui->clearButton->setEnabled(hasPoints);
}

void VWindowPolygon::getPolygon(QPolygonF &polygon) const
{
    polygon.clear();
    int polySize = getPolygonSize();
    for(int i = 0; i < polySize; ++i)
    {
        polygon.append(QPointF(m_qvX[i], m_qvY[i]));
    }
}

int VWindowPolygon::getPolygonSize() const
{
    return std::min(m_qvX.size(), m_qvY.size());
}

void VWindowPolygon::closeEvent(QCloseEvent *)
{
    emit windowClosed();
}

void VWindowPolygon::on_buttonBox_rejected()
{
    reject();
}

void VWindowPolygon::on_buttonBox_accepted()
{
    accept();
}

void VWindowPolygon::pl_on_before_report()
{
    int width, height;
    width = ui->plotWidget->width();
    height = ui->plotWidget->height();
    if (height < width)
    {
        double axSize = ui->plotWidget->yAxis->range().size();
        double axLower = ui->plotWidget->xAxis->range().lower;
        double axUpper = axLower + axSize * (static_cast<double>(width) / height);
        ui->plotWidget->xAxis->setRange(axLower, axUpper);
    }
    else
    {
        double axSize = ui->plotWidget->xAxis->range().size();
        double axLower = ui->plotWidget->yAxis->range().lower;
        double axUpper = axLower + axSize * (static_cast<double>(height) / width);
        ui->plotWidget->yAxis->setRange(axLower, axUpper);
    }
}

void VWindowPolygon::pl_on_mouse_release(QMouseEvent* event)
{
    if (!m_plotDragged && event->button() == Qt::LeftButton)
    {
        QPoint point = event->pos();
        double coordX = ui->plotWidget->xAxis->pixelToCoord(point.x());
        double coordY = ui->plotWidget->yAxis->pixelToCoord(point.y());
        addPoint(coordX, coordY);
    }
    m_mousePressed = false;
    m_plotDragged = false;
}

void VWindowPolygon::pl_on_mouse_press(QMouseEvent* event)
{
    m_mousePressed = true;
}

void VWindowPolygon::pl_on_mouse_move(QMouseEvent* event)
{
    if (m_mousePressed)
        m_plotDragged = true;
}

void VWindowPolygon::on_undoButton_clicked()
{
    removeLast();
}

void VWindowPolygon::on_clearButton_clicked()
{
    reset();
}

void VWindowPolygon::on_resetViewButton_clicked()
{
    resetView();
}

void VWindowPolygon::on_enclosedButton_clicked()
{
    plotEnclosed();
}

void VWindowPolygon::on_exportButton_clicked()
{
    //TODO
}
