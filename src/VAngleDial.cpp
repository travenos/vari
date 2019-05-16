/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <cmath>

#include <QPainter>
#include <QMouseEvent>

#include "VAngleDial.h"

VAngleDial::VAngleDial(QWidget *parent) :
    QWidget(parent)
{
    repaint();
}

VAngleDial::~VAngleDial() {}

void VAngleDial::setRange(double min, double max)
{
    min = _normalizeAngle(min);
    max = _normalizeAngle(max);
    if (min > max)
        std::swap(min, max);
    m_min = min;
    m_max = max;
    setAngle(m_angle);
}

void VAngleDial::setMin(double min)
{
    min = _normalizeAngle(min);
    m_min = min;
    if (min > m_max)
        m_max = min;
    setAngle(m_angle);
}

void VAngleDial::setMax(double max)
{
    max = _normalizeAngle(max);
    m_min = max;
    if (max < m_min)
        m_min = max;
    setAngle(m_angle);
}

void VAngleDial::setLineWidth(double width)
{
    m_lineWidth = std::max(width, 1.0);
    repaint();
}

void VAngleDial::setArrowSizeCoef(double coef)
{
    m_arrowSizeCoef = std::min(std::max(coef, 0.0), 1.0);
    repaint();
}

void VAngleDial::setColor(const QColor &color)
{
    m_color = color;
    repaint();
}

void VAngleDial::setAngle(double angle)
{
    angle = _normalizeAngle(angle);
    angle = std::min(angle, m_max);
    angle = std::max(angle, m_min);
    if (angle != m_angle)
    {
        m_angle = angle;
        this->repaint();
        emit angleChanged(angle);
    }
}

void VAngleDial::paintEvent(QPaintEvent *)
{
    static const double ARROW_ANGLE{M_PI / 4};

    m_center.setX(this->width() / 2.0);
    m_center.setY(this->height() / 2.0);
    m_radius = std::min(m_center.x(), m_center.y());

    double angle{-m_angle * M_PI / 180};
    double sinA{sin(angle)};
    double cosA{cos(angle)};
    double rCosA{m_radius * cosA}, rSinA{m_radius * sinA};
    QPointF point1{m_center.x() - rCosA, m_center.y() - rSinA};
    QPointF point2{m_center.x() + rCosA, m_center.y() + rSinA};

    double arrowSize{m_arrowSizeCoef * m_radius};

    QPointF arrowP1 = point2 + QPointF(arrowSize * (cos(angle - M_PI - ARROW_ANGLE) - cosA),
                       arrowSize * (sin(angle - M_PI - ARROW_ANGLE) - sinA));
    QPointF arrowP2 = point2 + QPointF(arrowSize * (cos(angle - M_PI + ARROW_ANGLE) - cosA),
                       arrowSize * (sin(angle - M_PI + ARROW_ANGLE) - sinA));

    QPolygonF arrowHead;
    arrowHead << point2 << arrowP1 << arrowP2;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(m_color, m_lineWidth, Qt::SolidLine, Qt::SquareCap));
    painter.drawLine(point1, point2); //Draw arrow line
    painter.drawEllipse(m_center, m_radius, m_radius); //Draw circle
    painter.setBrush(Qt::SolidPattern);
    painter.drawPolygon(arrowHead); //Draw arrow head
}

void VAngleDial::mouseMoveEvent(QMouseEvent *event)
{
    if (_isInsideCircle(event->x(), event->y()) && m_mousePressed)
    {
        double angle{_getCorrectAngle(event->x(), event->y())};
        setAngle(angle);
    }
}

void VAngleDial::mousePressEvent(QMouseEvent *event)
{
    if (_isInsideCircle(event->x(), event->y()))
    {
        m_mousePressed = true;
        double angle{_getCorrectAngle(event->x(), event->y())};
        setAngle(angle);
    }
}

void VAngleDial::mouseReleaseEvent(QMouseEvent *)
{
    m_mousePressed = false;
}

/**
 * Private methods
 */

double inline VAngleDial::_getCorrectAngle(double x, double y) const
{
    double x0 {x - m_center.x()};
    double y0 {m_center.y() - y};
    return atan2(y0, x0) * 180.0 / M_PI;
}

inline bool VAngleDial::_isInsideCircle(double x, double y) const
{
    return (_square(x - m_center.x()) + _square(y - m_center.y()) <= _square(m_radius));
}

constexpr inline double VAngleDial::_square(double x)
{
    return x * x;
}

double VAngleDial::_normalizeAngle(double angle)
{
    angle = fmod(angle, 360.0);
    int sgnA = (angle >= 0) ? 1 : -1;
    if (fabs(angle) > 180.0)
        angle -= 360.0 * sgnA;
    return angle;
}
