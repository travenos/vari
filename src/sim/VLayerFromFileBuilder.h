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
                      const VSimulationParametres::const_ptr &p_simParam,
                      VUnit units=M);
virtual ~VLayerFromFileBuilder();
const VLayer::ptr &build();

protected: 
    std::map<int, VSimNode::ptr> m_nodesMap;
    VSimNode::vector_ptr m_pNodes;
    VSimTriangle::vector_ptr m_pTriangles;
    QFile m_file;
    VUnit m_units;
    
void addShapePart(std::list<int> *connectionList) ;
void createConnections(std::list<int> *vertices) ;
void addNode(int id, QVector3D pos) ;
void makeNeighbours(int nodeId, int neighborId) ;
virtual bool importNodes()  = 0;
virtual bool importConnections()  = 0;

private:
void addTriangle(int nodeId1, int nodeId2, int nodeId3) ;
inline void addQuadrangle(int nodeId1, int nodeId2, int nodeId3, int nodeId4) ;
};

#endif //_VLAYERFROMFILEBUILDER_H
