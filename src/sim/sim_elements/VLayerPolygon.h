/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VLAYER_POLYGON_H
#define _VLAYER_POLYGON_H

#include <vector>

#include <QVector2D>
#include <QPolygonF>

#include "VSimTriangle.h"

class VLayerPolygon
{
public:
    VLayerPolygon(const VSimTriangle::const_list_ptr &triangles,
                  const QVector2D &min, const QVector2D &max, float step);
    VLayerPolygon() = default;
    virtual ~VLayerPolygon();

    void reset();
    void reset(const VSimTriangle::const_list_ptr &triangles,
               const QVector2D &min, const QVector2D &max, float step);

    float getStep() const;
    bool isEmpty() const;

    const std::vector<QPolygonF> & getPolygons() const;
    bool pointIsInside(const QVector2D & point) const;
    bool pointIsInside(const QPointF & point) const;

    static const float MIN_STEP;

private:
    void createPolygons(const VSimTriangle::const_list_ptr &triangles);

    inline bool getIndexes(const QVector2D &pos, int &i, int &j) const;
    inline QPointF getPoint(int i, int j) const;

    bool pointIsInsideRecursion(const QPointF & point, int fromContour, bool even) const;

    float m_step{MIN_STEP};
    QVector2D m_min;
    QVector2D m_max;
    std::array<int, 2> m_arrSizes{{0, 0}};
    std::vector<QPolygonF> m_polygons;
    std::vector<std::array<int, 4>> m_hierarchy;
};

#endif //_VLAYER_POLYGON_H
