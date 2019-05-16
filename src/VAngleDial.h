/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VANGLEDIAL_H
#define _VANGLEDIAL_H

#include <QWidget>

class VAngleDial : public QWidget
{
    Q_OBJECT
public:
    explicit VAngleDial(QWidget *parent = nullptr);
    virtual ~VAngleDial();
    inline double getAngle() const {return m_angle;}
    inline double getMin() const {return m_min;}
    inline double getMax() const {return m_max;}
    inline double getLineWidth() const {return m_lineWidth;}
    inline double getArrowSizeCoef() const {return m_arrowSizeCoef;}
    inline const QColor & getColor() const {return m_color;}

    void setRange(double min, double max);
    void setMin(double min);
    void setMax(double max);
    void setLineWidth(double width);
    void setArrowSizeCoef(double coef);
    void setColor(const QColor &color);

public slots:
    void setAngle(double angle);

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
private:
    double _getCorrectAngle(double x, double y) const;
    bool _isInsideCircle(double x, double y) const;
    static constexpr double _square(double x);
    static double _normalizeAngle(double angle);

    double m_angle{0.0};
    double m_min{-180.0};
    double m_max{180.0};
    double m_lineWidth{1.0};
    double m_arrowSizeCoef{0.2};
    double m_radius{0.0};
    QPointF m_center{0.0, 0.0};
    QColor m_color{0, 0, 0};
    bool m_mousePressed{false};
private slots:

signals:
    void angleChanged(double angle);
};

#endif // _VANGLEDIAL_H
