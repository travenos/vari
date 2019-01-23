/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VNODES_VOLUME_H
#define _VNODES_VOLUME_H

#include <list>
#include <array>

#include "VSimNode.h"

class VNodesVolume
{
public: 

    VNodesVolume() = delete; //TODO
    VNodesVolume(const VSimNode::const_map_ptr &nodes);
    VNodesVolume(const VSimNode::const_map_ptr &nodes, float step);
    virtual ~VNodesVolume();
    VNodesVolume(const VNodesVolume& ) = delete; //TODO implement theese methods
    VNodesVolume& operator= (const VNodesVolume& ) = delete; //TODO implement theese methods
    //TODO Also implement move and rvalue

    float getAverageDistance() const;
    float getStep() const;
    void getSize(QVector3D &size) const;
    void getConstraints(QVector3D &min, QVector3D &max) const;

    void getPointsInSphere(VSimNode::list_t &nodesList, const QVector3D &point, float radius,
                           bool strict=false) const;
    void getPointsBetweenSpheres(VSimNode::list_t &nodesList, const QVector3D &point,
                                 float radius1, float radius2,
                                 bool strict=false) const;
    void getCylinder(VSimNode::list_t &nodesList, const QVector3D &point,
                     float radius, float height,
                     bool strict=false) const;
    VSimNode::ptr getNearestNode(const QVector3D &point) const;

    static inline float projectioXYLength(const QVector3D &vect);

    static const float MIN_STEP;

private: 
    inline void calculateAverageDistance(const VSimNode::const_map_ptr &nodes);
    inline void calcSizes(const VSimNode::const_map_ptr &nodes);
    inline void fillArray(const VSimNode::const_map_ptr &nodes);
    inline void allocate();
    inline void deallocate();

    inline bool getIndexes(const QVector3D &pos, int &i, int &j, int &k) const;

    float m_step{MIN_STEP};
    float m_averageDistance{0};
    QVector3D m_min;
    QVector3D m_max;
    QVector3D m_size;
    std::array<int, 3> m_arrSizes { {0, 0, 0} };
    VSimNode::list_t *** m_nodes{nullptr};
};

#endif //_VNODES_VOLUME_H
