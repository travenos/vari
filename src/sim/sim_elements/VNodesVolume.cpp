/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif

#include <QVector2D>

#include <algorithm>
#include <cmath>

#include "VNodesVolume.h"

/**
 * VNodesVolume implementation
 */

const float VNodesVolume::MIN_STEP{0.001f};
const float VNodesVolume::STEP_COEF{static_cast<float>(sqrt(2.0 / 3.0))};

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

VNodesVolume::VNodesVolume(const VNodesVolume& copy):
    m_step{copy.m_step},
    m_averageDistance{copy.m_averageDistance},
    m_min{copy.m_min},
    m_max{copy.m_max},
    m_size{copy.m_size},
    m_arrSizes(copy.m_arrSizes),
    m_nodes{copy.getNodesArrCopy()}
{
}

VNodesVolume& VNodesVolume::operator= (const VNodesVolume& copy)
{
    if(this != &copy)
    {
        m_step = copy.m_step;
        m_averageDistance = copy.m_averageDistance;
        m_min = copy.m_min;
        m_max = copy.m_max;
        m_size = copy.m_size;
        m_arrSizes = copy.m_arrSizes;
        m_nodes = copy.getNodesArrCopy();
    }
    return *this;
}

VNodesVolume::VNodesVolume(VNodesVolume&& temp):
    m_step{temp.m_step},
    m_averageDistance{temp.m_averageDistance},
    m_min{temp.m_min},
    m_max{temp.m_max},
    m_size{temp.m_size},
    m_arrSizes(temp.m_arrSizes),
    m_nodes{temp.getNodesArrCopy()}
{
    temp.m_nodes = nullptr;
    temp.reset();
}

VNodesVolume& VNodesVolume::operator=(VNodesVolume&& temp)
{
    m_step = temp.m_step;
    m_averageDistance = temp.m_averageDistance;
    m_min = temp.m_min;
    m_max = temp.m_max;
    m_size = temp.m_size;
    m_arrSizes = temp.m_arrSizes;
    m_nodes = temp.m_nodes;
    temp.m_nodes = nullptr;
    temp.reset();
    return *this;
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
    m_step = std::max(m_averageDistance * STEP_COEF, MIN_STEP);
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
        qInfo() << "VNodesVolume destroyed";
    #endif
}

inline void VNodesVolume::calculateAverageDistance(const VSimNode::const_map_ptr &nodes)
{
    double distance{0};
    int counter{0};
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
    for (size_t i{0}; i < m_arrSizes.size(); ++i)
    {
        m_arrSizes[i] = static_cast<int>(m_size[static_cast<int>(i)] / m_step + 1);
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
    QVector3D noOffset{pos - m_min};
    QVector3D backwardOffset{m_max - pos};
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

const QVector3D &VNodesVolume::getSize() const
{
    return m_size;
}

const QVector3D &VNodesVolume::getMin() const
{
    return m_min;
}

const QVector3D &VNodesVolume::getMax() const
{
    return m_max;
}

void VNodesVolume::getConstraints(QVector3D &min, QVector3D &max) const
{
    min = m_min;
    max = m_max;
}

float VNodesVolume::getMaxZ() const
{
    return m_max.z();
}

float VNodesVolume::getMinZ() const
{
    return m_min.z();
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

void VNodesVolume::getPointsInCylinder(VSimNode::list_t &nodesList,
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
                            && diffVector.toVector2D().length() <= radius)
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
    if (isEmpty())
        return VSimNode::ptr();
    int r = 0;
    int i0, j0, k0;
    getIndexes(point, i0, j0, k0);
    VSimNode::list_t nodesList;
    while(true)
    {
        int r_sq = r * r;
        for(int i{std::max(i0 - r, 0)}, endI{std::min(i0 + r, m_arrSizes[0] - 1)}; i <= endI; ++i)
        {
            int d_i_sq = (i - i0) * (i - i0);
            int r_j = static_cast<int>(sqrt(r_sq - d_i_sq) + 0.5);
            for(int j{std::max(j0 - r_j, 0)}, endJ{std::min(j0 + r_j, m_arrSizes[1] - 1)}; j <= endJ; ++j)
            {
                int d_j_sq = (j - j0) * (j - j0);
                int k_base = static_cast<int>(sqrt(r_sq - d_i_sq - d_j_sq) + 0.5);
                int k1 = k_base + k0;
                int k2 = -k_base + k0;
                if (k1 >= 0 && k1 <= m_arrSizes[2] - 1)
                    nodesList.insert(nodesList.end(), m_nodes[i][j][k1].begin(), m_nodes[i][j][k1].end());
                if (k2 >= 0 && k2 <= m_arrSizes[2] - 1)
                    nodesList.insert(nodesList.end(), m_nodes[i][j][k2].begin(), m_nodes[i][j][k2].end());
            }
        }
        if (nodesList.size() > 0)
            break;
        ++r;
    }
    auto compare = [&point] (const VSimNode::ptr &node1, const VSimNode::ptr &node2) -> bool
    {
        return node1->getDistance(point) < node2->getDistance(point);
    };
    VSimNode::ptr minNode = *std::min_element(nodesList.begin(), nodesList.end(), compare);
    return minNode;
}

bool VNodesVolume::isEmpty() const
{
    return (m_arrSizes[0] <= 0);
}

inline VSimNode::list_t *** VNodesVolume::getNodesArrCopy() const
{
    VSimNode::list_t *** nodesCopy;
    nodesCopy = new VSimNode::list_t ** [ m_arrSizes[0] ];
    for (int i = 0; i < m_arrSizes[0]; ++i)
    {
        nodesCopy[i] = new VSimNode::list_t * [ m_arrSizes[1] ];
        for (int j = 0; j < m_arrSizes[1]; ++j)
        {
            nodesCopy[i][j] = new VSimNode::list_t[ m_arrSizes[2] ];
            std::copy(m_nodes[i][j], m_nodes[i][j] + m_arrSizes[2], nodesCopy[i][j]);
        }
    }
    return nodesCopy;
}
