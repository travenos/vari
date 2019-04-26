/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSIMNODE_H
#define _VSIMNODE_H

#include <QVector3D>
#include <vector>
#include <list>
#include <unordered_map>
#include <memory>
#include <atomic>

#include "VSimElement.h"

class VSimTriangle;

class VSimNode: public VSimElement
{
public: 
    static const uint LAYERS_NUMBER{2};

    static const double CONSIDERED_FULL;

    typedef std::shared_ptr<VSimNode> ptr;
    typedef std::shared_ptr<const VSimNode> const_ptr;
    typedef std::vector<ptr> vector_t;
    typedef std::shared_ptr<vector_t> vector_ptr;
    typedef std::shared_ptr<const vector_t> const_vector_ptr;
    typedef std::unordered_map<uint, ptr> map_t;
    typedef std::shared_ptr<map_t> map_ptr;
    typedef std::shared_ptr<const map_t> const_map_ptr;
    typedef std::list<ptr> list_t;
    typedef std::shared_ptr<list_t> list_ptr;
    typedef std::shared_ptr<const list_t> const_list_ptr;

    typedef std::list< std::pair<double, VSimNode*> > neighbours_list_t;
    typedef neighbours_list_t layered_neighbours_t[LAYERS_NUMBER];

    enum VLayerSequence { CURRENT=0, OTHER };
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
    void commit(bool *p_madeChanges=nullptr, bool *p_isFull=nullptr) ;
    double getPressure() const override;
    double getFilledPart() const override;
    bool isFull() const;

    void addNeighbourMutually(VSimNode *node, VLayerSequence layer=CURRENT) ;
    void addNeighbourMutually(const VSimNode::ptr &node, VLayerSequence layer=CURRENT);
    void addNeighbourMutually(VSimNode *node, VLayerSequence layer, float dist) ;
    void addNeighbourMutually(const VSimNode::ptr &node, VLayerSequence layer, float dist);
    void addNeighbour(VSimNode *node, VLayerSequence layer=CURRENT) ;
    void addNeighbour(const VSimNode::ptr &node, VLayerSequence layer=CURRENT);
    void addNeighbour(VSimNode *node, VLayerSequence layer, float dist) ;
    void addNeighbour(const VSimNode::ptr &node, VLayerSequence layer, float dist);
    void clearAllNeighbours() ;
    void clearNeighbours(VLayerSequence layer) ;
    void clearAllNeighboursMutually() ;
    void clearNeighboursMutually(VLayerSequence layer) ;

    float getDistance(const VSimNode* node) const ;
    float getDistance(const VSimNode::const_ptr &node) const;
    float getDistance(const QVector3D& point) const;
    float getDistance(const QVector2D &point) const;
    const QVector3D &getPosition() const ;
    void setPosition(const QVector3D& pos);
    void reset() override;
    void setBoundaryPressures(double injectionPressure, double vacuumPressure);
    void getNeighbours(std::vector<const VSimNode *> &neighbours) const ;
    void getNeighbours(std::vector<const VSimNode *> &neighbours, VLayerSequence layer) const ;
    void getNeighbours(neighbours_list_t &neighbours) const ;
    void getNeighbours(neighbours_list_t &neighbours,
                       VLayerSequence layer) const ;
    const layered_neighbours_t &getNeighbours() const;
    const neighbours_list_t &getNeighbours(VLayerSequence layer) const;
    void getNeighboursId(std::vector<uint> &neighbourId) const ;
    void getNeighboursId(std::vector<uint> &neighbourId, VLayerSequence layer) const;
    size_t getNeighboursNumber() const ;
    size_t getNeighboursNumber(VLayerSequence layer) const ;
    double getCavityHeight() const ;
    double getPorosity() const ;
    double getPermeability() const ;

    void removeNeighbour(uint id);
    void removeNeighbour(const VSimNode* node);
    void isolateNode();
    void isolateAndMarkForRemove();

    bool isMarkedForRemove() const override;
    bool isInjection() const override;
    bool isVacuum() const override;
    bool isNormal() const override;

private:
    inline size_t calcNeighboursNumber() const;

    layered_neighbours_t m_neighbours;
    QVector3D m_position;
    double m_injectionPressure;
    double m_vacuumPressure;
    std::atomic<double> m_currentPressure;
    double m_newPressure;
    VNodeRole m_role;
    size_t m_neighboursNumber;
    bool m_removeMark;
};

#endif //_VSIMNODE_H
