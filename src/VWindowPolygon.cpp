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
const QString VWindowPolygon::INTERSECTION_ERROR("Заданный контур содержит пересчения");


VWindowPolygon::VWindowPolygon(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VWindowPolygon),
    m_mousePressed(false),
    m_plotDragged(false)
{
    ui->setupUi(this);

    ui->vertexEditFrame->setVisible(false);

    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,0);

    m_pUndoShortcut = new QShortcut(QKeySequence("Ctrl+Z"), this);
    QObject::connect(m_pUndoShortcut, SIGNAL(activated()), this, SLOT(on_undoButton_clicked()));

    updateButtonsStates();
    resetView();

    ui->stepSpinBox->setMinimum(MIN_CHARACTERISTIC_LENGTH);
    ui->stepSpinBox->setLocale(QLocale::C);

    //TODO set minimums and maximums
    ui->addXSpinBox->setLocale(QLocale::C);
    ui->addYSpinBox->setLocale(QLocale::C);
    ui->changeXSpinBox->setLocale(QLocale::C);
    ui->changeYSpinBox->setLocale(QLocale::C);

    ui->plotWidget->xAxis->setLabel(QStringLiteral("x, m"));
    ui->plotWidget->yAxis->setLabel(QStringLiteral("y, m"));
    ui->plotWidget->setInteraction(QCP::iRangeDrag, true);
    ui->plotWidget->setInteraction(QCP::iRangeZoom, true);
    connect(ui->plotWidget, SIGNAL(beforeReplot()), this, SLOT(pl_on_before_report()));
    connect(ui->plotWidget, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(pl_on_mouse_release(QMouseEvent*)));
    connect(ui->plotWidget, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(pl_on_mouse_press(QMouseEvent*)));
    connect(ui->plotWidget, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(pl_on_mouse_move(QMouseEvent*)));

    m_pPlotCurve = new QCPCurve(ui->plotWidget->xAxis, ui->plotWidget->yAxis);
    m_pPlotCurve->setScatterStyle(QCPScatterStyle::ssPlusCircle);
    m_pPlotCurve->setPen(QColor(Qt::blue));
    m_pCloseCurve = new QCPCurve(ui->plotWidget->xAxis, ui->plotWidget->yAxis);
    m_pCloseCurve->setPen(QColor(Qt::red));
    m_pHighlightCurve = new QCPCurve(ui->plotWidget->xAxis, ui->plotWidget->yAxis);;
    QCPScatterStyle highlightStyle(QCPScatterStyle::ssCircle, QColor(Qt::green), QColor(Qt::green), 10);
    m_pHighlightCurve->setScatterStyle(highlightStyle);
    m_pHighlightCurve->setPen(QColor(Qt::blue));

    loadSavedParameters();
}

VWindowPolygon::~VWindowPolygon()
{
    saveParameters();
    m_pPlotCurve->deleteLater();
    m_pCloseCurve->deleteLater();
    m_pHighlightCurve->deleteLater();
    m_pUndoShortcut->deleteLater();
    delete ui;
    #ifdef DEBUG_MODE
        qInfo() << "VWindowPolygon destroyed";
    #endif
}

void VWindowPolygon::accept()
{
    if(!lastLineCausesIntersection())
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
    else
        showIntersectionError();
}

void VWindowPolygon::reset()
{
    m_qvT.clear();
    m_qvX.clear();
    m_qvY.clear();
    ui->verticesListWidget->clear();
    plot();
    m_pHighlightCurve->data()->clear();
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

void VWindowPolygon::addVertex(double x, double y)
{
    m_qvT.append(m_qvT.size());
    m_qvX.append(x);
    m_qvY.append(y);
    plotEnclosed();
    addVertexToList(getPolygonSize() - 1);
    updateButtonsStates();
}

void VWindowPolygon::removeVertex(int index)
{
    if (index >= 0 && index < getPolygonSize())
    {
        removeVertexFromList(index);
        m_qvT.remove(index);
        m_qvX.remove(index);
        m_qvY.remove(index);
        plotEnclosed();
        updateButtonsStates();
        if (getPolygonSize() == 0)
        {
            m_pHighlightCurve->data()->clear();
        }
    }
}

void VWindowPolygon::removeLast()
{
    removeVertex(getPolygonSize() - 1);
}

void VWindowPolygon::changeVertex(int index, double x, double y)
{
    if (index >= 0 && index < getPolygonSize())
    {
        m_qvX.replace(index, x);
        m_qvY.replace(index, y);
        plotEnclosed();
        updateVertexRecord(index);
    }
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
    if(!lastLineCausesIntersection())
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
    else
        showIntersectionError();
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

void VWindowPolygon::showIntersectionError()
{
    QMessageBox::warning(this, ERROR_TITLE, INTERSECTION_ERROR);
}

bool VWindowPolygon::pointCausesIntersection(double x, double y) const
{
    int polygonSize = getPolygonSize();
    if (polygonSize <= 2)
    {
        return false;
    }
    int lastIndex = polygonSize - 1;
    QLineF lastLine(m_qvX[lastIndex], m_qvY[lastIndex], x, y);
    for(int i = 1; i < lastIndex; ++i)
    {
        QLineF line(m_qvX[i - 1], m_qvY[i - 1], m_qvX[i], m_qvY[i]);
        if (line.intersect(lastLine, nullptr) == QLineF::BoundedIntersection)
            return true;
    }
    return false;
}

bool VWindowPolygon::lastLineCausesIntersection() const
{
    int polygonSize = getPolygonSize();
    if (polygonSize <= 3)
    {
        return false;
    }
    int lastIndex = polygonSize - 1;
    QLineF lastLine(m_qvX[lastIndex], m_qvY[lastIndex], m_qvX[0], m_qvY[0]);
    for(int i = 2; i < lastIndex; ++i)
    {
        QLineF line(m_qvX[i - 1], m_qvY[i - 1], m_qvX[i], m_qvY[i]);
        if (line.intersect(lastLine, nullptr) == QLineF::BoundedIntersection)
            return true;
    }
    return false;
}

QString VWindowPolygon::getVertexString(double x, double y) const
{
    QString pointStr = QStringLiteral("X")
            + QString::number(x, 'f', 3)
            + QStringLiteral(" Y")
            + QString::number(y, 'f', 3);
    return pointStr;
}

void VWindowPolygon::highlight(int index)
{
    if (index >= 0 && index < getPolygonSize())
    {
        m_pHighlightCurve->data()->clear();
        QVector<double> t{m_qvT.at(index)};
        QVector<double> x{m_qvX.at(index)};
        QVector<double> y{m_qvY.at(index)};
        m_pHighlightCurve->setData(t, x, y, true);
        ui->plotWidget->replot();
        ui->plotWidget->update();
    }
}

void VWindowPolygon::selectVertex()
{
    int vertexNumber = ui->verticesListWidget->currentRow();
    if (ui->verticesListWidget->currentIndex().isValid()
            && vertexNumber < getPolygonSize())
    {
        ui->vertexEditFrame->setVisible(true);
        highlight(vertexNumber);
        showCoords(vertexNumber);
    }
    else
        ui->vertexEditFrame->setVisible(false);
}

void VWindowPolygon::showCoords(int index)
{
    if (index >= 0 && index < getPolygonSize())
    {
        ui->changeXSpinBox->setValue(m_qvX.at(index));
        ui->changeYSpinBox->setValue(m_qvY.at(index));
        ui->restoreCoordsButton->setEnabled(false);
    }
}

void VWindowPolygon::addVertexToList(int index)
{
    if (index >= 0 && index < getPolygonSize())
    {
        QString pointStr = getVertexString(m_qvX.at(index), m_qvY.at(index));
        ui->verticesListWidget->addItem(pointStr);
        ui->verticesListWidget->setCurrentRow(ui->verticesListWidget->count() - 1);
    }
}

void VWindowPolygon::removeVertexFromList(int index)
{
    if (index >= 0 && index < ui->verticesListWidget->count())
    {
        if (index == ui->verticesListWidget->currentRow())
        {
            if(index > 0)
                ui->verticesListWidget->setCurrentRow(index - 1);
        }
        delete ui->verticesListWidget->item(index);
    }
}

void VWindowPolygon::updateVertexRecord(int index)
{
    if (index >= 0 && index < ui->verticesListWidget->count())
    {
        if (index < getPolygonSize())
        {
            QString pointStr = getVertexString(m_qvX.at(index), m_qvY.at(index));
            ui->verticesListWidget->item(index)->setText(pointStr);
            if (index == ui->verticesListWidget->currentRow())
            {
                selectVertex();
            }
        }
        else
        {
            delete ui->verticesListWidget->item(index);
        }
    }
}

/**
 * Slots
 */

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
        if (!pointCausesIntersection(coordX, coordY))
            addVertex(coordX, coordY);
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

void VWindowPolygon::on_verticesListWidget_itemSelectionChanged()
{
    selectVertex();
}

void VWindowPolygon::on_restoreCoordsButton_clicked()
{
    selectVertex();
}

void VWindowPolygon::on_changeCoordsButton_clicked()
{
    int index = ui->verticesListWidget->currentRow();
    if (ui->verticesListWidget->currentIndex().isValid() &&
            index >= 0 && index < getPolygonSize())
    {

        double x = ui->changeXSpinBox->value();
        double y = ui->changeYSpinBox->value();
        if (!pointCausesIntersection(x, y))
            changeVertex(index, x, y);
        else
        {
            showIntersectionError();
            showCoords(index);
        }
    }
}

void VWindowPolygon::on_removeVertexButton_clicked()
{
    int index = ui->verticesListWidget->currentRow();
    if (ui->verticesListWidget->currentIndex().isValid() &&
            index >= 0 && index < getPolygonSize())
    {
        removeVertex(index);
    }
}

void VWindowPolygon::on_changeXSpinBox_valueChanged(double arg1)
{
    ui->restoreCoordsButton->setEnabled(true);
}

void VWindowPolygon::on_changeYSpinBox_valueChanged(double arg1)
{
    ui->restoreCoordsButton->setEnabled(true);
}

void VWindowPolygon::on_addVertexButton_clicked()
{
    double x = ui->addXSpinBox->value();
    double y = ui->addYSpinBox->value();
    if (!pointCausesIntersection(x, y))
    {
        addVertex(x, y);
    }
    else
    {
        showIntersectionError();
    }
}
