/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VLayerFromFileBuilder.h"
#include "structures/VExceptions.h"

/**
 * VLayerFromFileBuilder implementation
 */


/**
 * @param filename
 * @param param VSimParam::const_ptr
 */
VLayerFromFileBuilder::VLayerFromFileBuilder(const QString &filename,
                                             const VCloth &material,
                                             VUnit units) :
    VLayerAbstractBuilder(material),
    m_pNodes(new VSimNode::map_t),
    m_pTriangles(new VSimTriangle::list_t),
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
    m_pLayer->setMinMaxIds(m_nodeStartId, m_nodeMaxId, m_triangleStartId, m_triangleMaxId);
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
    uint currentId = id + m_nodeStartId;
    if (currentId > m_nodeMaxId)
        m_nodeMaxId = currentId;
    VSimNode::ptr newNode(new VSimNode(currentId, pos, m_pMaterial));
    m_pNodes->insert(std::make_pair(currentId, newNode));
}

void VLayerFromFileBuilder::makeNeighbours(int nodeId, int neighborId) 
{
    if(nodeId == neighborId)
        return;
    nodeId += m_nodeStartId;
    neighborId += m_nodeStartId;
    if(m_pNodes->find(nodeId) != m_pNodes->end() && m_pNodes->find(neighborId) != m_pNodes->end())
    {
        VSimNode::ptr node = (*m_pNodes)[nodeId];
        VSimNode::ptr neighbor = (*m_pNodes)[neighborId];
        node->addNeighbourMutually(neighbor.get());
    }
}

void VLayerFromFileBuilder::addTriangle(int nodeId1, int nodeId2, int nodeId3) 
{
    nodeId1 += m_nodeStartId;
    nodeId2 += m_nodeStartId;
    nodeId3 += m_nodeStartId;
    VSimNode::const_ptr node1 = (m_pNodes->find(nodeId1))->second;
    VSimNode::const_ptr node2 = (m_pNodes->find(nodeId2))->second;
    VSimNode::const_ptr node3 = (m_pNodes->find(nodeId3))->second;
    uint currentId = static_cast<uint>(m_pTriangles->size()) + m_triangleStartId;
    if (currentId > m_triangleMaxId)
        m_triangleMaxId = currentId;
    VSimTriangle::ptr triangle(new VSimTriangle(currentId, m_pMaterial, node1,node2,node3));
    m_pTriangles->push_back(triangle);
}

inline void VLayerFromFileBuilder::addQuadrangle(int nodeId1, int nodeId2, int nodeId3, int nodeId4) 
{
    addTriangle(nodeId1,nodeId2,nodeId3);
    addTriangle(nodeId3,nodeId4,nodeId1);
}
