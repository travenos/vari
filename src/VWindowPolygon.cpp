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

#include "sim/structures/VExceptions.h"
#include "sim/layer_builders/VLayerManualBuilder.h"

const QCPRange VWindowPolygon::X_RANGE(-1, 1);
const QCPRange VWindowPolygon::Y_RANGE(-0.5, 0.5);
const int VWindowPolygon::MIN_POLYGON_SIZE = 3;
const double VWindowPolygon::MIN_CHARACTERISTIC_LENGTH = 0.001;
const double VWindowPolygon::DEFAULT_CHARACTERISTIC_LENGTH = 0.01;
const double VWindowPolygon::MIN_CHARACTERISTIC_RATIO = 0.005;

const QString VWindowPolygon::SAVE_FILE_DIALOG_TITLE("Создание и сохранение слоя в файл");
const QString VWindowPolygon::FILE_DIALOG_FORMATS("Файлы gmsh(*.msh);;"
                                                  "Все файлы (*)");
const QString VWindowPolygon::ERROR_TITLE("Ошибка");
const QString VWindowPolygon::EXPORT_TO_FILE_ERROR("Ошибка сохранения в файл");
const QString VWindowPolygon::TOO_SMALL_STEP_ERROR("Задан слишком маленький средний шаг сетки для текущих"
                                                   " габаритов слоя. Необходимо увеличить шаг сетки"
                                                   " как минимум в %1 раз");


VWindowPolygon::VWindowPolygon(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VWindowPolygon),
    m_mousePressed(false),
    m_plotDragged(false)
{
    ui->setupUi(this);

    m_pUndoShortcut = new QShortcut(QKeySequence("Ctrl+Z"), this);
    QObject::connect(m_pUndoShortcut, SIGNAL(activated()), this, SLOT(on_undoButton_clicked()));

    updateButtonsStates();
    resetView();

    ui->stepSpinBox->setMinimum(MIN_CHARACTERISTIC_LENGTH);
    ui->stepSpinBox->setLocale(QLocale::C);

    ui->plotWidget->xAxis->setLabel(QStringLiteral("x, м"));
    ui->plotWidget->yAxis->setLabel(QStringLiteral("y, м"));
    ui->plotWidget->setInteraction(QCP::iRangeDrag, true);
    ui->plotWidget->setInteraction(QCP::iRangeZoom, true);
    connect(ui->plotWidget, SIGNAL(beforeReplot()), this, SLOT(pl_on_before_report()));
    connect(ui->plotWidget, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(pl_on_mouse_release(QMouseEvent*)));
    connect(ui->plotWidget, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(pl_on_mouse_press(QMouseEvent*)));
    connect(ui->plotWidget, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(pl_on_mouse_move(QMouseEvent*)));

    m_pPlotCurve = new QCPCurve(ui->plotWidget->xAxis, ui->plotWidget->yAxis);
    m_pPlotCurve->setScatterStyle(QCPScatterStyle::ssPlusCircle);
    m_pCloseCurve = new QCPCurve(ui->plotWidget->xAxis, ui->plotWidget->yAxis);
    m_pCloseCurve->setPen(QColor(Qt::red));

    loadSavedParameters();
}

VWindowPolygon::~VWindowPolygon()
{
    saveParameters();
    m_pPlotCurve->deleteLater();
    m_pCloseCurve->deleteLater();
    m_pUndoShortcut->deleteLater();
    delete ui;
    #ifdef DEBUG_MODE
        qInfo() << "VWindowPolygon destroyed";
    #endif
}

void VWindowPolygon::accept()
{
    double ratio = getStepRatio();
    if (ratio >= MIN_CHARACTERISTIC_RATIO)
    {
        hide();
        QPolygonF polygon;
        getPolygon(polygon);
        emit polygonAvailable(polygon, m_characteristicLength);
        close();
    }
    else
        showRatioError(ratio);
}

void VWindowPolygon::reset()
{
    m_qvT.clear();
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
    m_qvT.append(m_qvT.size());
    m_qvX.append(x);
    m_qvY.append(y);
    plotEnclosed();
    updateButtonsStates();
}

void VWindowPolygon::removeLast()
{
    if (m_qvT.size() > 0)
        m_qvT.pop_back();
    if (m_qvX.size() > 0)
        m_qvX.pop_back();
    if (m_qvY.size() > 0)
        m_qvY.pop_back();
    plotEnclosed();
    updateButtonsStates();
}

void VWindowPolygon::plot()
{
    m_pPlotCurve->setData(m_qvT, m_qvX, m_qvY, true);
    m_pCloseCurve->data()->clear();
    ui->plotWidget->replot();
    ui->plotWidget->update();
}

void VWindowPolygon::plotEnclosed()
{
    QVector<double> coordX, coordY, coordT;
    if (m_qvT.size() >= MIN_POLYGON_SIZE)
    {
        coordT.push_back(0);
        coordT.push_back(1);
    }
    if (m_qvX.size() >= MIN_POLYGON_SIZE)
    {
        coordX.push_back(m_qvX.last());
        coordX.push_back(m_qvX.first());
    }
    if (m_qvY.size() >= MIN_POLYGON_SIZE)
    {
        coordY.push_back(m_qvY.last());
        coordY.push_back(m_qvY.first());
    }
    m_pPlotCurve->setData(m_qvT, m_qvX, m_qvY, true);
    m_pCloseCurve->setData(coordT, coordX, coordY, true);
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

void VWindowPolygon::exportMeshToFile(const QString &filename) const
{
    QPolygonF polygon;
    getPolygon(polygon);
    VLayerManualBuilder::exportToFile(polygon, filename, m_characteristicLength);
}

void VWindowPolygon::meshExportProcedure()
{
    double ratio = getStepRatio();
    if (ratio >= MIN_CHARACTERISTIC_RATIO)
    {
        QString filename = QFileDialog::getSaveFileName(this, SAVE_FILE_DIALOG_TITLE, m_lastDir,
                                                        FILE_DIALOG_FORMATS);
        if (!filename.isEmpty())
        {
            m_lastDir = QFileInfo(filename).absolutePath();
            try
            {
                exportMeshToFile(filename);
            }
            catch (VExportException)
            {
                QMessageBox::warning(this, ERROR_TITLE, EXPORT_TO_FILE_ERROR);
            }
            QSettings settings;
            settings.setValue(QStringLiteral("import/lastDir"), m_lastDir);
            settings.sync();
        }
    }
    else
        showRatioError(ratio);
}

void VWindowPolygon::loadSavedParameters()
{
    QSettings settings;
    m_lastDir = settings.value(QStringLiteral("import/lastDir"), QDir::homePath()).toString();
    bool ok;
    m_characteristicLength = settings.value(QStringLiteral("mesh/meshStep"), DEFAULT_CHARACTERISTIC_LENGTH).toDouble(&ok);
    if (!ok || m_characteristicLength < MIN_CHARACTERISTIC_LENGTH)
        m_characteristicLength = DEFAULT_CHARACTERISTIC_LENGTH;
    showCharacteristicLength();
}

void VWindowPolygon::saveParameters() const
{
    QSettings settings;
    settings.setValue(QStringLiteral("import/lastDir"), m_lastDir);
    settings.setValue(QStringLiteral("mesh/meshStep"), m_characteristicLength);
    settings.sync();
}

void VWindowPolygon::showCharacteristicLength()
{
    ui->stepSpinBox->setValue(m_characteristicLength);
}

double VWindowPolygon::getStepRatio() const
{
    if (m_qvX.size() >= MIN_POLYGON_SIZE && m_qvY.size() >= MIN_POLYGON_SIZE)
    {
        auto xMinMax = std::minmax_element(m_qvX.begin(), m_qvX.end());
        auto yMinMax = std::minmax_element(m_qvY.begin(), m_qvY.end());
        double maxSide = std::max(*(xMinMax.second) - *(xMinMax.first),
                                  *(yMinMax.second) - *(yMinMax.first));
        double ratio = m_characteristicLength / maxSide;
        return ratio;
    }
    else
        return 0;
}

void VWindowPolygon::showRatioError(double ratio)
{
    double times = MIN_CHARACTERISTIC_RATIO / ratio;
    QMessageBox::warning(this, ERROR_TITLE,
                         TOO_SMALL_STEP_ERROR.arg(QString::number(times, 'g', 4)));
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

void VWindowPolygon::on_exportButton_clicked()
{
    meshExportProcedure();
}

void VWindowPolygon::on_stepSpinBox_valueChanged(double arg1)
{
    m_characteristicLength = arg1;
}
