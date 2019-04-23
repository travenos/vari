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

#include "sim/VSimulationFacade.h"
#include "sim/structures/VExceptions.h"
#include "sim/structures/VTable.h"
#include "sim/layer_builders/VLayerManualBuilder.h"

const QCPRange VWindowPolygon::X_RANGE(-1, 1);
const QCPRange VWindowPolygon::Y_RANGE(-0.5, 0.5);
const int VWindowPolygon::MIN_1D_POLYGON_SIZE{2};
const int VWindowPolygon::MIN_2D_POLYGON_SIZE{3};
const double VWindowPolygon::MIN_CHARACTERISTIC_LENGTH{0.001};
const double VWindowPolygon::DEFAULT_CHARACTERISTIC_LENGTH{0.03};
const double VWindowPolygon::MIN_CHARACTERISTIC_RATIO{0.005};

const QString VWindowPolygon::SAVE_FILE_DIALOG_TITLE("Создание и сохранение слоя в файл");
const QString VWindowPolygon::FILE_DIALOG_FORMATS("Файлы gmsh(*.msh);;"
                                                  "Все файлы (*)");
const QString VWindowPolygon::ERROR_TITLE("Ошибка");
const QString VWindowPolygon::EXPORT_TO_FILE_ERROR("Ошибка сохранения в файл");
const QString VWindowPolygon::TOO_SMALL_STEP_ERROR("Задан слишком маленький средний шаг сетки для текущих"
                                                   " габаритов слоя. Необходимо увеличить шаг сетки"
                                                   " как минимум в %1 раз");
const QString VWindowPolygon::INTERSECTION_ERROR("Заданный контур содержит пересчения");
const QString VWindowPolygon::TABLE_ERROR("Вершина лежит за пределами рабочего поля стенда");

const int VWindowPolygon::POINT_SIZE{6};
const int VWindowPolygon::HIGHLIGHT_POINT_SIZE{10};


VWindowPolygon::VWindowPolygon(QWidget *parent,
                               std::shared_ptr<const VSimulationFacade> p_facade) :
    QMainWindow(parent),
    ui(new Ui::VWindowPolygon),
    m_pFacade(p_facade),
    m_useTable(true),
    m_mode1D(false)
{
    ui->setupUi(this);

    ui->vertexEditFrame->setVisible(false);

    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,0);

    m_pUndoShortcut = new QShortcut(QKeySequence("Ctrl+Z"), this);
    QObject::connect(m_pUndoShortcut, SIGNAL(activated()), this, SLOT(on_undoButton_clicked()));

    updateButtonsStates();

    ui->stepSpinBox->setMinimum(MIN_CHARACTERISTIC_LENGTH);
    ui->stepSpinBox->setLocale(QLocale::C);

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
    QCPScatterStyle plotScatterStyle(QCPScatterStyle::ssPlusCircle, POINT_SIZE);
    m_pPlotCurve->setScatterStyle(plotScatterStyle);
    m_pPlotCurve->setPen(QColor(0,128,0));
    m_pCloseCurve = new QCPCurve(ui->plotWidget->xAxis, ui->plotWidget->yAxis);
    m_pCloseCurve->setPen(QColor(Qt::red));
    m_pHighlightCurve = new QCPCurve(ui->plotWidget->xAxis, ui->plotWidget->yAxis);;
    QCPScatterStyle highlightStyle(QCPScatterStyle::ssCircle, QColor(Qt::green), QColor(Qt::green), HIGHLIGHT_POINT_SIZE);
    m_pHighlightCurve->setScatterStyle(highlightStyle);
    m_pHighlightCurve->setPen(QColor(Qt::blue));

    m_pTableCurve = new QCPCurve(ui->plotWidget->xAxis, ui->plotWidget->yAxis);
    m_pTableCurve->setPen(QColor(Qt::black));
    m_pInjectionEllipse = new QCPItemEllipse(ui->plotWidget);
    m_pInjectionEllipse->setPen(QColor(Qt::blue));
    m_pVacuumEllipse = new QCPItemEllipse(ui->plotWidget);
    m_pVacuumEllipse->setPen(QColor(0,255,255));

    drawOtherPolygons();

    loadSavedParameters();
    updateTable();
    resetView();
}

VWindowPolygon::~VWindowPolygon()
{
    saveParameters();
    m_pPlotCurve->deleteLater();
    m_pCloseCurve->deleteLater();
    m_pHighlightCurve->deleteLater();
    m_pUndoShortcut->deleteLater();
    m_pTableCurve->deleteLater();
    m_pInjectionEllipse->deleteLater();
    m_pVacuumEllipse->deleteLater();
    for (auto &otherCurve: m_otherLayersCurves)
    {
        otherCurve->deleteLater();
    }
    delete ui;
    #ifdef DEBUG_MODE
        qInfo() << "VWindowPolygon destroyed";
    #endif
}

void VWindowPolygon::accept()
{
    if (!m_mode1D)
        accept2DProcedure();
    else
        accept1DProcedure();
}

void VWindowPolygon::accept2DProcedure()
{
    if(!lastLineCausesIntersection())
    {
        if (m_useTable)
        {
            int polySize = getPolygonSize();
            for (int i = 0; i < polySize; ++i)
            {
                if (!vertexIsOkForTable(m_qvX[i], m_qvY[i]))
                {
                    showTableError();
                    return;
                }
            }
        }
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

void VWindowPolygon::accept1DProcedure()
{
    QPolygonF polygon;
    getPolygonFrom1D(polygon);
    VPolygonQuality quality;
    if (!correctFor1D(polygon, &quality))
    {
        if (quality == INTERSECTION)
            showIntersectionError();
        else if (quality == OUT_OF_TABLE)
            showTableError();
        return;
    }
    hide();;
    emit polygonAvailable(polygon, m_characteristicLength);
    close();
}

bool VWindowPolygon::correctFor1D(const QPolygonF &polygon, VPolygonQuality * quality)
{
    for(int i{0}; i < polygon.size(); ++i)
    {
        const QPointF &point{polygon.at(i)};
        if (m_useTable && !vertexIsOkForTable(point.x(), point.y()))
        {
            if (quality != nullptr)
                (*quality) = OUT_OF_TABLE;
            return false;
        }
        if (vertexCausesIntersection(i, point.x(), point.y(), polygon))
        {
            if (quality != nullptr)
                (*quality) = INTERSECTION;
            return false;
        }
    }
    if (quality != nullptr)
        (*quality) = GOOD;
    return true;
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
    if (m_pFacade)
    {
        std::shared_ptr<const VTable> p_table{m_pFacade->getTable()};
        float sizeX{p_table->getSize().x()};
        float sizeY{p_table->getSize().y()};

        QCPRange xRange(0, sizeX);
        QCPRange yRange(0, sizeY);

        ui->plotWidget->xAxis->setRange(xRange);
        ui->plotWidget->yAxis->setRange(yRange);
    }
    else
    {
        ui->plotWidget->xAxis->setRange(X_RANGE);
        ui->plotWidget->yAxis->setRange(Y_RANGE);
    }
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
    double t{(!m_qvT.empty()) ? m_qvT.last() + 1 : 0};
    m_qvT.append(t);
    m_qvX.append(x);
    m_qvY.append(y);
    if (m_mode1D)
        plot();
    else
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
        if (getPolygonSize() == 0)
        {
            m_pHighlightCurve->data()->clear();
        }
        if (m_mode1D)
            plot();
        else
            plotEnclosed();
        updateButtonsStates();
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
        if (m_mode1D)
            plot();
        else
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
    if (m_qvT.size() >= MIN_2D_POLYGON_SIZE)
    {
        coordT.push_back(0);
        coordT.push_back(1);
    }
    if (m_qvX.size() >= MIN_2D_POLYGON_SIZE)
    {
        coordX.push_back(m_qvX.last());
        coordX.push_back(m_qvX.first());
    }
    if (m_qvY.size() >= MIN_2D_POLYGON_SIZE)
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
    int polySize{getPolygonSize()};
    bool hasValidPolygon{(!m_mode1D && polySize >= MIN_2D_POLYGON_SIZE)
                         || (m_mode1D && polySize >= MIN_1D_POLYGON_SIZE)};
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(hasValidPolygon);
    ui->exportButton->setEnabled(hasValidPolygon);
    bool hasPoints{polySize > 0};
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

void VWindowPolygon::getPolygonFrom1D(QPolygonF &polygon) const
{
    polygon.clear();
    float halfWidth{static_cast<float>(ui->tubeWidthSpinBox->value() / 2)};
    QPolygonF centralPolygon;
    getPolygon(centralPolygon);
    int polySize{centralPolygon.size()};
    if (polySize > 1)
    {
        QPointF point1, point2;
        QPolygonF otherSide;
        getPerpendicular(centralPolygon.at(0), centralPolygon.at(1), halfWidth, point1, point2);
        polygon.append(point1);
        otherSide.append(point2);
        for(int i{1}; i < polySize; ++i)
        {
            if (i < polySize - 1)
            {
                getBisectrix(centralPolygon.at(i-1),
                             centralPolygon.at(i),
                             centralPolygon.at(i+1),
                             halfWidth, point1, point2);
            }
            else
            {
                getPerpendicular(centralPolygon.at(i),
                                 centralPolygon.at(i-1),
                                 halfWidth, point1, point2);
            }
            QLineF section1(polygon.last(), point1);
            QLineF sectionCentral(centralPolygon.at(i-1), centralPolygon.at(i));
            if (section1.intersect(sectionCentral, nullptr) == QLineF::BoundedIntersection)
                std::swap(point1, point2);
            polygon.append(point1);
            otherSide.append(point2);
        }
        std::reverse(otherSide.begin(), otherSide.end());
        polygon.append(otherSide);
    }
    else if (polySize == 1)
    {
        polygon.append(QPointF(m_qvX.at(0), m_qvY.at(1) + halfWidth));
        polygon.append(QPointF(m_qvX.at(0), m_qvY.at(1) - halfWidth));
    }
}

void VWindowPolygon::getPerpendicular(const QPointF &inPoint1, const QPointF &inPoint2,
                                      float length,
                                      QPointF &outPoint1, QPointF &outPoint2)
{
    QVector2D inVector(inPoint2 - inPoint1);
    QVector2D outVector(-inVector.y(), inVector.x());
    outVector.normalize();
    outVector *= length;
    outPoint1 = inPoint1 + outVector.toPointF();
    outPoint2 = inPoint1 - outVector.toPointF();
}

void VWindowPolygon::getBisectrix(const QPointF &inPoint0, const QPointF &inPoint1,
                                  const QPointF &inPoint2, float length,
                                  QPointF &outPoint1, QPointF &outPoint2)
{
    QVector2D inVector1(inPoint0 - inPoint1);
    QVector2D inVector2(inPoint2 - inPoint1);
    QVector2D bisVector{inVector1.normalized() + inVector2.normalized()};
    if (!bisVector.isNull())
    {
        float angleCos{QVector2D::dotProduct(inVector1, bisVector)
                    / (inVector1.length() * bisVector.length())};
        float angleSin{static_cast<float>(sqrt(1 - angleCos * angleCos))};
        bisVector.normalize();
        if (angleSin != 0)
            bisVector *= length / angleSin;
        else
            bisVector *= length;
        outPoint1 = inPoint1 + bisVector.toPointF();
        outPoint2 = inPoint1 - bisVector.toPointF();
    }
    else
    {
        getPerpendicular(inPoint1, inPoint2, length, outPoint1, outPoint2);
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

void VWindowPolygon::exportMeshToFile(const QString &filename, const QPolygonF &polygon) const
{
    VLayerManualBuilder::exportToFile(polygon, filename, m_characteristicLength);
}

void VWindowPolygon::meshExportProcedure()
{
    QPolygonF polygon;
    if (!m_mode1D)
    {
        getPolygon(polygon);
        if (lastLineCausesIntersection())
        {
            showIntersectionError();
            return;
        }
        double ratio = getStepRatio();
        if (ratio < MIN_CHARACTERISTIC_RATIO)
        {
            showRatioError(ratio);
            return;
        }
    }
    else
    {
        getPolygonFrom1D(polygon);
        VPolygonQuality quality;
        if (!correctFor1D(polygon, &quality))
        {
            if (quality == INTERSECTION)
            {
                showIntersectionError();
                return;
            }
        }
    }
    QString filename = QFileDialog::getSaveFileName(this, SAVE_FILE_DIALOG_TITLE, m_lastDir,
                                                    FILE_DIALOG_FORMATS);
    if (!filename.isEmpty())
    {
        m_lastDir = QFileInfo(filename).absolutePath();
        try
        {
            exportMeshToFile(filename, polygon);
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

void VWindowPolygon::loadSavedParameters()
{
    QSettings settings;
    m_lastDir = settings.value(QStringLiteral("import/lastDir"), QDir::homePath()).toString();
    bool ok;
    m_characteristicLength = settings.value(QStringLiteral("mesh/meshStep"), DEFAULT_CHARACTERISTIC_LENGTH).toDouble(&ok);
    if (!ok || m_characteristicLength < MIN_CHARACTERISTIC_LENGTH)
        m_characteristicLength = DEFAULT_CHARACTERISTIC_LENGTH;
    bool useTable, mode1D;
    double tubeWidth;
    useTable = settings.value(QStringLiteral("mesh/useTable"), m_mode1D).toBool();
    mode1D = settings.value(QStringLiteral("mesh/mode1D"), m_mode1D).toBool();
    tubeWidth = settings.value(QStringLiteral("mesh/tubeWidth"), ui->tubeWidthSpinBox->value()).toDouble();
    setUseTable(useTable);
    set1DMode(mode1D);
    ui->tubeWidthSpinBox->setValue(tubeWidth);
    showCharacteristicLength();
}

void VWindowPolygon::saveParameters() const
{
    QSettings settings;
    settings.setValue(QStringLiteral("import/lastDir"), m_lastDir);
    settings.setValue(QStringLiteral("mesh/meshStep"), m_characteristicLength);
    settings.setValue(QStringLiteral("mesh/useTable"), m_useTable);
    settings.setValue(QStringLiteral("mesh/mode1D"), m_mode1D);
    settings.setValue(QStringLiteral("mesh/tubeWidth"), ui->tubeWidthSpinBox->value());
    settings.sync();
}

void VWindowPolygon::showCharacteristicLength()
{
    ui->stepSpinBox->setValue(m_characteristicLength);
}

double VWindowPolygon::getStepRatio() const
{
    if (m_qvX.size() >= MIN_2D_POLYGON_SIZE && m_qvY.size() >= MIN_2D_POLYGON_SIZE)
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

void VWindowPolygon::showTableError()
{
    QMessageBox::warning(this, ERROR_TITLE, TABLE_ERROR);
}

bool VWindowPolygon::newVertexCausesIntersection(double x, double y) const
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

bool VWindowPolygon::removingVertexCausesIntersection(int index) const
{
    int polySize = getPolygonSize();
    if (index > 0 && index < polySize - 1)
    {
        QLineF resultLine(m_qvX[index - 1], m_qvY[index - 1], m_qvX[index + 1], m_qvY[index + 1]);
        for(int i = 1; i < index - 1; ++i)
        {
            QLineF line(m_qvX[i - 1], m_qvY[i - 1], m_qvX[i], m_qvY[i]);
            if (line.intersect(resultLine, nullptr) == QLineF::BoundedIntersection)
                return true;
        }
        for(int i = index + 3; i < polySize; ++i)
        {
            QLineF line(m_qvX[i - 1], m_qvY[i - 1], m_qvX[i], m_qvY[i]);
            if (line.intersect(resultLine, nullptr) == QLineF::BoundedIntersection)
                return true;
        }
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

bool VWindowPolygon::vertexCausesIntersection(int index, double x, double y) const
{
    QPolygonF polygon;
    getPolygon(polygon);
    return vertexCausesIntersection(index, x, y, polygon);
}

bool VWindowPolygon::vertexCausesIntersection(int index, double x, double y,
                                              const QPolygonF &polygon)
{
    int polygonSize = polygon.size();
    if (index > 0)
    {
        QLineF checkLine(polygon.at(index - 1), QPointF(x, y));
        for(int i = 1; i < index - 1; ++i)
        {
            QLineF line(polygon.at(i - 1), polygon.at(i));
            if (line.intersect(checkLine, nullptr) == QLineF::BoundedIntersection)
                return true;
        }
        for(int i = index + 2; i < polygonSize; ++i)
        {
            QLineF line(polygon.at(i - 1), polygon.at(i));
            if (line.intersect(checkLine, nullptr) == QLineF::BoundedIntersection)
                return true;
        }
    }
    if (index < polygonSize - 1)
    {
        QLineF checkLine(QPointF(x, y), polygon.at(index + 1));
        for(int i = 1; i < index; ++i)
        {
            QLineF line(polygon.at(i - 1), polygon.at(i));
            if (line.intersect(checkLine, nullptr) == QLineF::BoundedIntersection)
                return true;
        }
        for(int i = index + 3; i < polygonSize; ++i)
        {
            QLineF line(polygon.at(i - 1), polygon.at(i));
            if (line.intersect(checkLine, nullptr) == QLineF::BoundedIntersection)
                return true;
        }
    }
    return false;
}

bool VWindowPolygon::vertexIsOkForTable(double x, double y) const
{
    if (m_useTable && m_pFacade)
    {
        std::shared_ptr<const VTable> p_table{m_pFacade->getTable()};
        float sizeX{p_table->getSize().x()};
        if (x < 0 || x > sizeX)
            return false;
        float sizeY{p_table->getSize().y()};
        if (y < 0 || y > sizeY)
            return false;
    }
    return true;
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

void VWindowPolygon::updateTable()
{
    if (m_pFacade)
    {
        std::shared_ptr<const VTable> p_table{m_pFacade->getTable()};
        m_pTableCurve->data()->clear();

        float sizeX{p_table->getSize().x()};
        float sizeY{p_table->getSize().y()};
        QVector<double> qvT{0, 1, 2, 3, 4};
        QVector<double> qvX{0, sizeX, sizeX, 0, 0};
        QVector<double> qvY{sizeY, sizeY, 0, 0, sizeY};
        m_pTableCurve->setData(qvT, qvX, qvY, true);

        float injectionRadius = p_table->getInjectionDiameter() / 2;
        float injectionX = p_table->getInjectionCoords().x();
        float injectionY = p_table->getInjectionCoords().y();
        m_pInjectionEllipse->topLeft->setCoords(injectionX - injectionRadius, injectionY - injectionRadius);
        m_pInjectionEllipse->bottomRight->setCoords(injectionX + injectionRadius, injectionY + injectionRadius);

        float vacuumRadius = p_table->getVacuumDiameter() / 2;
        float vacuumX = p_table->getVacuumCoords().x();
        float vacuumY = p_table->getVacuumCoords().y();
        m_pVacuumEllipse->topLeft->setCoords(vacuumX - vacuumRadius, vacuumY - vacuumRadius);
        m_pVacuumEllipse->bottomRight->setCoords(vacuumX + vacuumRadius, vacuumY + vacuumRadius);

        m_pInjectionEllipse->setVisible(true);
        m_pVacuumEllipse->setVisible(true);

        ui->plotWidget->replot();
        ui->plotWidget->update();
    }
    else
    {
        setUseTable(false);
        m_pTableCurve->data()->clear();
        m_pInjectionEllipse->setVisible(false);
        m_pVacuumEllipse->setVisible(false);

        ui->plotWidget->replot();
        ui->plotWidget->update();
    }
}

void VWindowPolygon::setUseTable(bool use)
{
    m_useTable = use;
    if (use != ui->useTableCheckBox->isChecked())
    {
        ui->useTableCheckBox->setChecked(use);
    }
}

void VWindowPolygon::drawOtherPolygons()
{
    for (auto &otherCurve: m_otherLayersCurves)
    {
        otherCurve->data()->clear();
        otherCurve->deleteLater();
    }
    m_otherLayersCurves.clear();
    if (m_pFacade)
    {
        std::vector<std::vector<QPolygonF> > otherLayersPolygons{m_pFacade->getAllActivePolygons()};
        for(auto & layerPolygons : otherLayersPolygons)
        {
            for (auto & polygon : layerPolygons)
            {
                QCPCurve * polyCurve = new QCPCurve(ui->plotWidget->xAxis, ui->plotWidget->yAxis);
                polyCurve->setPen(QColor(Qt::gray));
                QVector<double> qvT, qvX, qvY;
                qvT.reserve(polygon.size() + 1);
                qvX.reserve(polygon.size() + 1);
                qvY.reserve(polygon.size() + 1);
                for (int i{0}; i < polygon.size(); ++i)
                {
                    qvT.append(i);
                    qvX.append(polygon.at(i).x());
                    qvY.append(polygon.at(i).y());
                }
                if (polygon.size() > 0)
                {
                    qvT.append(qvT.size());
                    qvX.append(polygon.at(0).x());
                    qvY.append(polygon.at(0).y());
                }
                polyCurve->setData(qvT, qvX, qvY);
                m_otherLayersCurves.push_back(polyCurve);
            }
        }
    }
    ui->plotWidget->replot();
    ui->plotWidget->update();
}

void VWindowPolygon::set1DMode(bool set)
{
    ui->mode1DRadioButton->setChecked(set);
    ui->mode2DRadioButton->setChecked(!set);
    ui->tubeWidthSpinBox->setEnabled(set);
    m_mode1D = set;
    if (set)
        plot();
    else
        plotEnclosed();
    updateButtonsStates();
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
    if (height > width)
    {
        double axSize = ui->plotWidget->yAxis->range().size();
        double halfRange = axSize * (static_cast<double>(width) / height) / 2;
        double middle = (ui->plotWidget->xAxis->range().lower
                         + ui->plotWidget->xAxis->range().upper) / 2;
        double axLower = middle - halfRange;
        double axUpper = middle + halfRange;
        ui->plotWidget->xAxis->setRange(axLower, axUpper);
    }
    else
    {
        double axSize = ui->plotWidget->xAxis->range().size();
        double halfRange = axSize * (static_cast<double>(height) / width) /2;
        double middle = (ui->plotWidget->yAxis->range().lower
                         + ui->plotWidget->yAxis->range().upper) / 2;
        double axLower = middle - halfRange;
        double axUpper = middle + halfRange;
        ui->plotWidget->yAxis->setRange(axLower, axUpper);
    }
}

void VWindowPolygon::pl_on_mouse_release(QMouseEvent* event)
{
    if (m_mouseInfo.pressedInCorrectPlace && !m_mouseInfo.plotDragged
            && !m_mouseInfo.vertexSelected
            && event->button() == Qt::LeftButton)
    {
        QPoint point = event->pos();
        double coordX = ui->plotWidget->xAxis->pixelToCoord(point.x());
        double coordY = ui->plotWidget->yAxis->pixelToCoord(point.y());
        if (!newVertexCausesIntersection(coordX, coordY) && vertexIsOkForTable(coordX, coordY))
            addVertex(coordX, coordY);
    }
    m_mouseInfo.mousePressed = false;
    m_mouseInfo.plotDragged = false;
    m_mouseInfo.vertexSelected = false;
}

void VWindowPolygon::pl_on_mouse_press(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return;
    m_mouseInfo.mousePressed = true;
    m_mouseInfo.vertexSelected = false;
    m_mouseInfo.pressedInCorrectPlace = false;
    QPoint pressPoint = event->pos();
    for (int i = getPolygonSize() - 1; i >= 0; --i)
    {
        QPoint vertexPoint;
        vertexPoint.setX(ui->plotWidget->xAxis->coordToPixel(m_qvX.at(i)));
        vertexPoint.setY(ui->plotWidget->yAxis->coordToPixel(m_qvY.at(i)));
        if ((vertexPoint - pressPoint).manhattanLength() <= POINT_SIZE)
        {
            ui->verticesListWidget->setCurrentRow(i);
            m_mouseInfo.vertexSelected = true;
            m_mouseInfo.selectedIndex = i;
            break;
        }
    }
    if (!m_mouseInfo.vertexSelected)
    {
        double pressCoordX = ui->plotWidget->xAxis->pixelToCoord(pressPoint.x());
        double pressCoordY = ui->plotWidget->yAxis->pixelToCoord(pressPoint.y());
        if (!newVertexCausesIntersection(pressCoordX, pressCoordY) && vertexIsOkForTable(pressCoordX, pressCoordY))
        {
            m_mouseInfo.pressedInCorrectPlace = true;
        }
    }
}

void VWindowPolygon::pl_on_mouse_move(QMouseEvent* event)
{
    if (m_mouseInfo.mousePressed && m_mouseInfo.vertexSelected)
    {
        m_mouseInfo.plotDragged = true;
        if (m_mouseInfo.selectedIndex >= 0)
        {
            QPoint point = event->pos();
            double x = ui->plotWidget->xAxis->pixelToCoord(point.x());
            double y = ui->plotWidget->yAxis->pixelToCoord(point.y());
            if (!vertexCausesIntersection(m_mouseInfo.selectedIndex, x, y)
                    && vertexIsOkForTable(x, y))
                changeVertex(m_mouseInfo.selectedIndex, x, y);
        }
    }
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
        bool noIntersections = !vertexCausesIntersection(index, x, y);
        bool okForTable = vertexIsOkForTable(x, y);
        if (noIntersections && okForTable)
            changeVertex(index, x, y);
        else
        {
            if (!noIntersections)
                showIntersectionError();
            if (!okForTable)
                showTableError();
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
        if (!removingVertexCausesIntersection(index))
            removeVertex(index);
        else
            showIntersectionError();
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
    bool noIntersections = !newVertexCausesIntersection(x, y);
    bool okForTable = vertexIsOkForTable(x, y);
    if ( noIntersections && okForTable )
    {
        addVertex(x, y);
    }
    else
    {
        if (!noIntersections)
            showIntersectionError();
        if (!okForTable)
            showTableError();
    }
}

void VWindowPolygon::on_useTableCheckBox_clicked(bool checked)
{
    setUseTable(checked);
}

void VWindowPolygon::on_mode1DRadioButton_toggled(bool checked)
{
    set1DMode(checked);
}
