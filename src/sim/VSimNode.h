/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMNODE_H
#define _VSIMNODE_H

#include <QVector3D>
#include <vector>
#include <memory>
#include <atomic>

#include "VSimElement.h"

class VSimTriangle;

class VSimNode: public VSimElement
{
public: 
    static const uint LAYERS_NUMBER = 3;

    typedef std::shared_ptr<VSimNode> ptr;
    typedef std::shared_ptr<const VSimNode> const_ptr;
    typedef std::shared_ptr<std::vector<VSimNode::ptr> > vector_ptr;
    typedef std::shared_ptr<const std::vector<VSimNode::ptr> > const_vector_ptr;
    typedef std::vector< std::pair<double, const VSimNode*> > neighbours_vector_t;
    typedef neighbours_vector_t layered_neighbours_t[LAYERS_NUMBER];

    enum VLayerSequence { PREVIOUS=0, CURRENT, NEXT };
    enum VNodeRole { NORMAL, INJECTION, VACUUM };

    VSimNode(uint id, const QVector3D &pos,
             const VCloth::const_ptr &p_material,
             double injectionPressure=0,
             double vacuumPressure=0,
             double pressure=0,
             VNodeRole role=NORMAL);

    virtual ~VSimNode();

    void setRole(VNodeRole role) ;
    VNodeRole getRole() const ;
    void setNewPressure(double newPressure) ;
    void commit(bool *madeChanges=nullptr, bool *isFull=nullptr) ;
    double getPressure() const override;
    double getFilledPart() const override;

    void addNeighbourMutually(VSimNode *node, VLayerSequence layer=CURRENT) ;
    void addNeighbour(const VSimNode *node, VLayerSequence layer=CURRENT) ;
    void clearAllNeighbours() ;

    void clearNeighbours(VLayerSequence layer) ;

    double getDistance(const VSimNode* node) const ;
    double getDistance(const QVector3D& point) const;
    const QVector3D &getPosition() const ;
    void reset() override;
    void setBoundaryPressures(double injectionPressure, double vacuumPressure);
    void getNeighbours(std::vector<const VSimNode *> &neighbours) const ;
    void getNeighbours(std::vector<const VSimNode *> &neighbours, VLayerSequence layer) const ;
    void getNeighbours(neighbours_vector_t &neighbours) const ;
    void getNeighbours(neighbours_vector_t &neighbours,
                       VLayerSequence layer) const ;
    const layered_neighbours_t &getNeighbours() const;
    const neighbours_vector_t &getNeighbours(VLayerSequence layer) const;
    void getNeighboursId(std::vector<uint> &neighbourId) const ;
    void getNeighboursId(std::vector<uint> &neighbourId, VLayerSequence layer) const;
    size_t getNeighboursNumber() const ;
    size_t getNeighboursNumber(VLayerSequence layer) const ;
    double getCavityHeight() const ;
    double getPorosity() const ;
    double getPermeability() const ;

    bool isInjection() const override;
    bool isVacuum() const override;
    bool isNormal() const override;

private:
    layered_neighbours_t m_neighbours;
    QVector3D m_position;
    double m_injectionPressure;
    double m_vacuumPressure;
    std::atomic<double> m_currentPressure;
    double m_newPressure;
    VNodeRole m_role;
    size_t m_neighboursNumber;
};

#endif //_VSIMNODE_H
