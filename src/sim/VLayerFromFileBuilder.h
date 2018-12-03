/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERFROMFILEBUILDER_H
#define _VLAYERFROMFILEBUILDER_H

#include <map>
#include <QFile>
#include "VLayerAbstractBuilder.h"
#include "VSimNode.h"
#include "VSimTriangle.h"
#include "VSimulationParametres.h"

class VLayerFromFileBuilder: public VLayerAbstractBuilder {
public: 
    
/**
 * @param filename
 * @param param VSimParam::const_ptr
 */
VLayerFromFileBuilder(const QString &filename,
                      const VCloth &material,
                      const VSimulationParametres::const_ptr &p_simParam);
virtual ~VLayerFromFileBuilder();
const VLayer::ptr &build();

protected: 
    std::map<int, VSimNode::ptr> m_nodesMap;
    VSimNode::vector_ptr m_pNodes;
    VSimTriangle::vector_ptr m_pTriangles;
    QFile m_file;
    
void addShapePart(std::list<int> *connectionList) noexcept;
void createConnections(std::list<int> *vertices) noexcept;
void addNode(int id, const QVector3D &pos) noexcept;
void makeNeighbours(int nodeId, int neighborId) noexcept;
virtual bool importNodes() noexcept = 0;
virtual bool importConnections() noexcept = 0;

private:
void addTriangle(int nodeId1, int nodeId2, int nodeId3) noexcept;
inline void addQuadrangle(int nodeId1, int nodeId2, int nodeId3, int nodeId4) noexcept;
};

#endif //_VLAYERFROMFILEBUILDER_H
