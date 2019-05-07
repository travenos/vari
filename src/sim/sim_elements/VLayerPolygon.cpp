/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#include <opencv2/imgcodecs.hpp>
#endif

#include <cmath>

#include <QPolygonF>

#include <opencv2/imgproc.hpp>

#include "VLayerPolygon.h"

/**
 * VLayerPolygon implementation
 */

const float VLayerPolygon::MIN_STEP{0.0001f};
const double VLayerPolygon::APPROX_EPSILON{3.0};

VLayerPolygon::VLayerPolygon()
{
    m_arrSizes.fill(0);
}

VLayerPolygon::VLayerPolygon(const VSimTriangle::const_list_ptr &triangles,
                             const QVector2D &min, const QVector2D &max, float step)
{
    reset(triangles, min, max, step);
}

void VLayerPolygon::reset()
{
    m_step = MIN_STEP;
    m_min.setX(0);
    m_min.setY(0);
    m_max.setX(0);
    m_max.setY(0);
    m_arrSizes.fill(0);
    m_polygons.clear();
    m_hierarchy.clear();
    m_polygons.shrink_to_fit();
    m_hierarchy.shrink_to_fit();
}

void VLayerPolygon::reset(const VSimTriangle::const_list_ptr &triangles,
                         const QVector2D &min, const QVector2D &max, float step)
{
    m_step = std::max(step, MIN_STEP);
    m_min = min;
    m_max = max;

    QVector2D size = m_max - m_min;
    for (size_t i{0}; i < m_arrSizes.size(); ++i)
    {
        m_arrSizes[i] = static_cast<int>(size[static_cast<int>(i)] / m_step + 1);
    }
    createPolygons(triangles);
}

VLayerPolygon::~VLayerPolygon()
{
      #ifdef DEBUG_MODE
          qInfo() << "VLayerPolygon destroyed";
      #endif
}

float VLayerPolygon::getStep() const
{
    return m_step;
}

bool VLayerPolygon::isEmpty() const
{
    return (m_arrSizes[0] <= 0);
}

inline bool VLayerPolygon::getIndexes(const QVector2D &pos,
                                     int &i, int &j) const
{
    QVector2D noOffset{pos - m_min};
    QVector2D backwardOffset{m_max - pos};
    i = static_cast<int>(std::round(noOffset.x() / m_step));
    j = static_cast<int>(std::round(noOffset.y() / m_step));
    for (int i{0}; i < 2; ++i)
    {
        if (noOffset[i] < 0 || backwardOffset[i] < 0)
            return false;
    }
    return true;
}

inline QPointF VLayerPolygon::getPoint(int i, int j) const
{
    QVector2D result(i * m_step, j * m_step);
    result += m_min;
    return result.toPointF();
}

const std::vector<QPolygonF> & VLayerPolygon::getPolygons() const
{
    return m_polygons;
}

bool VLayerPolygon::pointIsInside(const QVector2D & point) const
{
    return pointIsInside(point.toPointF());
}

bool VLayerPolygon::pointIsInside(const QPointF & point) const
{
    return pointIsInsideRecursion(point, 0, true);
}

bool VLayerPolygon::pointIsInsideRecursion(const QPointF & point, int fromContour, bool even) const
{
    if(fromContour >= 0 && fromContour < static_cast<int>(m_polygons.size()))
    {
        if (m_polygons.at(fromContour).containsPoint(point, Qt::OddEvenFill))
            return pointIsInsideRecursion(point, m_hierarchy.at(fromContour).at(3), !even);
        else
            return pointIsInsideRecursion(point, m_hierarchy.at(fromContour).at(0), even);
    }
    else
        return !even;
}

void VLayerPolygon::createPolygons(const VSimTriangle::const_list_ptr &triangles)
{
    cv::Scalar FILL_COLOR(255);
    const uint VERTICES_NUMBER{VSimTriangle::VERTICES_NUMBER};
    cv::Mat img(std::vector<int>({m_arrSizes.at(1), m_arrSizes.at(0)}), CV_8U, cv::Scalar(0));
    for (auto & p_triange : *triangles)
    {
        std::vector<std::vector<cv::Point> > pts(1);
        pts.at(0).resize(VERTICES_NUMBER);
        QVector3D vertices[VERTICES_NUMBER];
        p_triange->getVertices(vertices);
        for (uint vertexIndex{0}; vertexIndex < VERTICES_NUMBER; ++vertexIndex)
        {
            int x, y;
            getIndexes(vertices[vertexIndex].toVector2D(), x, y);
            pts.at(0).at(vertexIndex).x = x;
            pts.at(0).at(vertexIndex).y = y;
        }
        cv::drawContours(img, pts, 0, FILL_COLOR, -1, cv::LINE_8);
    }

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(img, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_TC89_KCOS);

    m_polygons.clear();
    m_hierarchy.clear();
    m_polygons.reserve(contours.size());
    m_hierarchy.reserve(hierarchy.size());
    for (size_t i{0}, size{std::min(contours.size(), hierarchy.size())}; i < size; ++i)
    {
        const std::vector<cv::Point> &currentContour = contours.at(i);
        std::vector<cv::Point> approxContour;
        cv::approxPolyDP(currentContour, approxContour, APPROX_EPSILON, true);
        m_polygons.push_back(QPolygonF());
        m_polygons.back().reserve(static_cast<int>(contours.size()));
        for (size_t j{0}, contourSize{approxContour.size()}; j < contourSize; ++j)
        {
            QPointF point{getPoint(approxContour.at(j).x, approxContour.at(j).y)};
            m_polygons.back().append(point);
        }
        const cv::Vec4i &currentHierarch = hierarchy.at(i);
        m_hierarchy.push_back({currentHierarch[0], currentHierarch[1],
                               currentHierarch[2], currentHierarch[3]});
    }
}
