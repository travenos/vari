/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VLayerFromFileBuilder.h"
#include "core/VExceptions.h"

/**
 * VLayerFromFileBuilder implementation
 */


/**
 * @param filename
 * @param param VSimParam::const_ptr
 */
VLayerFromFileBuilder::VLayerFromFileBuilder(const QString &filename,
                                             const VCloth &material,
                                             const VSimulationParametres::const_ptr &p_simParam,
                                             VUnit units) :
    VLayerAbstractBuilder(material, p_simParam),
    m_pNodes(new std::vector<VSimNode::ptr>),
    m_pTriangles(new std::vector<VSimTriangle::ptr>),
    m_file(filename),
    m_units(units)
{
}

VLayerFromFileBuilder::~VLayerFromFileBuilder()
{}

/**
 * @return VLayer::ptr
 */
const VLayer::ptr &VLayerFromFileBuilder::build()
{
    m_file.open(QIODevice::ReadOnly);
    if (!importNodes())
    {
        m_file.close();
        throw VImportException();
    }
    if (!importConnections())
    {
        m_file.close();
        throw VImportException();
    }
    m_file.close();
    m_pLayer.reset(new VLayer(m_pNodes, m_pTriangles, m_pMaterial));
    return m_pLayer;
}

void VLayerFromFileBuilder::addShapePart(std::list<int>* connectionList)  
{
    if(connectionList->size() == 3)
    {
        //we have a triangle
        std::list<int>::const_iterator it = connectionList->begin();
        int node1 = *it;
        int node2 = *(++it);
        int node3 = *(++it);
        addTriangle(node1,node2,node3);
    }
    else if(connectionList->size() == 4)
    {
        //we have a quadrangle
        std::list<int>::const_iterator it = connectionList->begin();
        int node1 = *it;
        int node2 = *(++it);
        int node3 = *(++it);
        int node4 = *(++it);
        addQuadrangle(node1,node2,node3,node4);
    }
}

void VLayerFromFileBuilder::createConnections(std::list<int>* vertices) 
{
    if(vertices->size() >= 2)
    {
        for(std::list<int>::const_iterator it = vertices->begin(); it != vertices->end(); it++)
        {
            std::list<int>::const_iterator itNode2 = it;
            itNode2++;
            if(itNode2 == vertices->end())
                itNode2 = vertices->begin();
            makeNeighbours(*it,*itNode2);
        }
        addShapePart(vertices);
    }
}

void VLayerFromFileBuilder::addNode(int id, QVector3D pos)
{
    if(m_units == MM)
        pos /= MM_IN_M;
    VSimNode::ptr newNode(new VSimNode(pos, m_pMaterial, m_pParam));
    m_nodesMap.insert(std::make_pair(id, newNode));
    m_pNodes->push_back(newNode);
}

void VLayerFromFileBuilder::makeNeighbours(int nodeId, int neighborId) 
{
    if(nodeId == neighborId)
        return;
    if(m_nodesMap.find(nodeId) != m_nodesMap.end() && m_nodesMap.find(neighborId) != m_nodesMap.end())
    {
        VSimNode::ptr node = m_nodesMap[nodeId];
        VSimNode::ptr neighbor = m_nodesMap[neighborId];
        node->addNeighbourMutually(neighbor.get());
    }
}

void VLayerFromFileBuilder::addTriangle(int nodeId1, int nodeId2, int nodeId3) 
{
    VSimNode::const_ptr node1 = (m_nodesMap.find(nodeId1))->second;
    VSimNode::const_ptr node2 = (m_nodesMap.find(nodeId2))->second;
    VSimNode::const_ptr node3 = (m_nodesMap.find(nodeId3))->second;
    VSimTriangle::ptr triangle(new VSimTriangle(m_pMaterial, m_pParam, node1,node2,node3));
    m_pTriangles->push_back(triangle);
}

inline void VLayerFromFileBuilder::addQuadrangle(int nodeId1, int nodeId2, int nodeId3, int nodeId4) 
{
    addTriangle(nodeId1,nodeId2,nodeId3);
    addTriangle(nodeId3,nodeId4,nodeId1);
}
