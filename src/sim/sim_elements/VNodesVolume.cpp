/**
 * Project VARI
 * @author Alexey Barashkov
 */
#ifdef DEBUG_MODE
#include <QDebug>
#endif

#include <algorithm>
#include <cmath>

#include "VNodesVolume.h"

const float VNodesVolume::MIN_STEP = 0.00001f;

VNodesVolume::VNodesVolume()
{
    reset();
}

VNodesVolume::VNodesVolume(const VSimNode::const_map_ptr &nodes)
{
    reset(nodes);
}

VNodesVolume::VNodesVolume(const VSimNode::const_map_ptr &nodes, float step)
{
    reset(nodes, step);
}

void VNodesVolume::reset()
{
    deallocate();
    m_nodes = nullptr;
    m_step = MIN_STEP;
    m_averageDistance = 0;
    m_min = QVector3D(0, 0, 0);
    m_max = QVector3D(0, 0, 0);
    m_size = QVector3D(0, 0, 0);
    m_arrSizes.fill(0);
}

void VNodesVolume::reset(const VSimNode::const_map_ptr &nodes)
{
    deallocate();
    calculateAverageDistance(nodes);
    m_step = (m_averageDistance / 2 >= MIN_STEP) ? m_averageDistance / 2 : MIN_STEP;
    calcSizes(nodes);
    allocate();
    fillArray(nodes);
}

void VNodesVolume::reset(const VSimNode::const_map_ptr &nodes, float step)
{
    deallocate();
    calculateAverageDistance(nodes);
    m_step = (step >= MIN_STEP) ? step : MIN_STEP;
    calcSizes(nodes);
    allocate();
    fillArray(nodes);
}

VNodesVolume::~VNodesVolume()
{
    deallocate();
    #ifdef DEBUG_MODE
        qInfo() << "VNodusVolume destroyed";
    #endif
}

inline void VNodesVolume::calculateAverageDistance(const VSimNode::const_map_ptr &nodes)
{
    double distance = 0;
    int counter = 0;
    for(auto &node : *nodes)
    {
        const VSimNode::neighbours_list_t &neighbours = node.second->getNeighbours(VSimNode::CURRENT);
        for(auto &neighbour : neighbours)
        {
            distance += node.second->getDistance(neighbour.second);
            ++counter;
        }
    }
    if (counter > 0)
        distance /= counter;
   m_averageDistance = distance;
}

inline void VNodesVolume::calcSizes(const VSimNode::const_map_ptr &nodes)
{
    if (nodes->size() > 0)
    {
        const QVector3D &firstPos = nodes->begin()->second->getPosition();
        m_min = firstPos;
        m_max = firstPos;
        for(auto &node : *nodes)
        {
            const QVector3D &pos = node.second->getPosition();
            if (pos.x() < m_min.x())
                m_min.setX(pos.x());
            else if (pos.x() > m_max.x())
                m_max.setX(pos.x());
            if (pos.y() < m_min.y())
                m_min.setY(pos.y());
            else if (pos.y() > m_max.y())
                m_max.setY(pos.y());
            if (pos.z() < m_min.z())
                m_min.setZ(pos.z());
            else if (pos.z() > m_max.z())
                m_max.setZ(pos.z());
        }
        m_size = m_max - m_min;
    }
    for (int i = 0; i < 3; ++i)
    {
        m_arrSizes[i] = static_cast<int>(m_size[i] / m_step + 1);
    }
}


inline void VNodesVolume::fillArray(const VSimNode::const_map_ptr &nodes)
{
    for(auto &node : *nodes)
    {
        const QVector3D &pos = node.second->getPosition();
        int i, j, k;
        getIndexes(pos, i, j, k);
        m_nodes[i][j][k].push_back(node.second);
    }
}

inline void VNodesVolume::allocate()
{
    m_nodes = new VSimNode::list_t ** [ m_arrSizes[0] ];
    for (int i = 0; i < m_arrSizes[0]; ++i)
    {
        m_nodes[i] = new VSimNode::list_t * [ m_arrSizes[1] ];
        for (int j = 0; j < m_arrSizes[1]; ++j)
        {
            m_nodes[i][j] = new VSimNode::list_t[ m_arrSizes[2] ];
        }
    }
}

inline void VNodesVolume::deallocate()
{
    if (m_nodes != nullptr)
    {
        for (int i = 0; i < m_arrSizes[0]; ++i)
        {
            for (int j = 0; j < m_arrSizes[1]; ++j)
            {
                delete [] m_nodes[i][j];
            }
            delete [] m_nodes[i];
        }
        delete m_nodes;
    }
}

inline bool VNodesVolume::getIndexes(const QVector3D &pos,
                                     int &i, int &j, int &k) const
{
    QVector3D noOffset = pos - m_min;
    QVector3D backwardOffset = m_max - pos;
    i = static_cast<int>(noOffset.x() / m_step);
    j = static_cast<int>(noOffset.y() / m_step);
    k = static_cast<int>(noOffset.z() / m_step);
    for (int i = 0; i < 3; ++i)
    {
        if (noOffset[i] < 0 || backwardOffset[i] < 0)
            return false;
    }
    return true;
}

float VNodesVolume::getAverageDistance() const
{
    return m_averageDistance;
}

float VNodesVolume::getStep() const
{
    return m_step;
}

void VNodesVolume::getSize(QVector3D &size) const
{
    size = m_size;
}

void VNodesVolume::getConstraints(QVector3D &min, QVector3D &max) const
{
    min = m_min;
    max = m_max;
}

void VNodesVolume::getPointsInSphere(VSimNode::list_t &nodesList,
                                     const QVector3D &point, float radius,
                                     bool strict) const
{
    nodesList.clear();
    int i0, j0, k0, r;
    getIndexes(point, i0, j0, k0);
    r = static_cast<int>(radius / m_step + 1);
    int r_sq = r * r;
    for (int i = std::max(i0 - r, 0); i <= std::min(i0 + r, m_arrSizes[0] - 1); ++i)
    {
        int d_i_sq = (i - i0) * (i - i0);
        int jBorder = static_cast<int>(sqrt(r_sq - d_i_sq));
        for (int j = std::max(j0 - jBorder, 0); j <= std::min(j0 + jBorder, m_arrSizes[1] - 1); ++j)
        {
            int d_j_sq = (j - j0) * (j - j0);
            int kBorder = static_cast<int>(sqrt(r_sq - d_i_sq - d_j_sq));
            for (int k = std::max(k0 - kBorder, 0); k <= std::min(k0 + kBorder, m_arrSizes[2] - 1); ++k)
            {
                for (auto &nodePtr : m_nodes[i][j][k])
                {
                    if (!strict || nodePtr->getDistance(point) <= radius)
                        nodesList.push_back(nodePtr);
                }
            }
        }
    }
}

void VNodesVolume::getPointsBetweenSpheres(VSimNode::list_t &nodesList,
                                           const QVector3D &point, float radius1, float radius2,
                                           bool strict) const
{
    nodesList.clear();
    int i0, j0, k0, r1, r2;
    getIndexes(point, i0, j0, k0);
    float radius = std::max(radius1, radius2);
    float internalRadius = std::min(radius1, radius2);
    r1 = static_cast<int>(radius / m_step + 1);
    r2 = static_cast<int>(internalRadius / m_step);
    int r1_sq = r1 * r1;
    int r2_sq = r2 * r2;
    for (int i = std::max(i0 - r1, 0); i <= std::min(i0 + r1, m_arrSizes[0] - 1); ++i)
    {
        int d_i_sq = (i - i0) * (i - i0);
        int jBorder = static_cast<int>(sqrt(r1_sq - d_i_sq));
        for (int j = std::max(j0 - jBorder, 0); j <= std::min(j0 + jBorder, m_arrSizes[1] - 1); ++j)
        {
            int d_j_sq = (j - j0) * (j - j0);
            int kBorder = static_cast<int>(sqrt(r1_sq - d_i_sq - d_j_sq));
            for (int k = std::max(k0 - kBorder, 0); k <= std::min(k0 + kBorder, m_arrSizes[2] - 1); ++k)
            {
                int d_k_sq = (k - k0) * (k - k0);
                if (d_i_sq + d_j_sq + d_k_sq >= r2_sq)
                    for (auto &nodePtr : m_nodes[i][j][k])
                    {
                        if (!strict)
                            nodesList.push_back(nodePtr);
                        else
                        {
                            float dist = nodePtr->getDistance(point);
                            if (dist <= radius1 && dist >= radius2)
                                nodesList.push_back(nodePtr);
                        }
                    }
            }
        }
    }
}

void VNodesVolume::getCylinder(VSimNode::list_t &nodesList,
                               const QVector3D &point, float radius, float height,
                               bool strict) const
{
    nodesList.clear();
    int i0, j0, k0, r, half_h;
    getIndexes(point, i0, j0, k0);
    r = static_cast<int>(radius / m_step + 1);
    half_h = static_cast<int>(height / (m_step * 2) + 1);
    int r_sq = r * r;
    for (int k = std::max(k0 - half_h, 0); k <= std::min(k0 + half_h, m_arrSizes[2] - 1); ++k)
        for (int i = std::max(i0 - r, 0); i <= std::min(i0 + r, m_arrSizes[0] - 1); ++i)
        {
            int d_i_sq = (i - i0) * (i - i0);
            int jBorder = static_cast<int>(sqrt(r_sq - d_i_sq));
            for (int j = std::max(j0 - jBorder, 0); j <= std::min(j0 + jBorder, m_arrSizes[1] - 1); ++j)
            {
                for (auto &nodePtr : m_nodes[i][j][k])
                {
                    if (!strict)
                        nodesList.push_back(nodePtr);
                    else
                    {
                        const QVector3D &nodePos = nodePtr->getPosition();
                        QVector3D diffVector = nodePos - point;
                        if (fabs(diffVector.z()) <= height
                            && projectioXYLength(diffVector) <= radius)
                        {
                            nodesList.push_back(nodePtr);
                        }
                    }
                }
            }
        }
}

VSimNode::ptr VNodesVolume::getNearestNode(const QVector3D &point) const
{
    //TODO
    return VSimNode::ptr();
}

inline float VNodesVolume::projectioXYLength(const QVector3D &vect)
{
    return sqrt(vect.x() * vect.x() + vect.y() * vect.y());
}
