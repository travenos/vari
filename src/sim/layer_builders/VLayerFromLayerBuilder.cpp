/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include "VLayerFromLayerBuilder.h"

/**
 * VLayerFromLayerBuilder implementation
 */

VLayerFromLayerBuilder::VLayerFromLayerBuilder(const VLayer::const_ptr &sourceLayerPtr) :
    VLayerAbstractBuilder(*(sourceLayerPtr->getMaterial()), sourceLayerPtr->getName()),
    m_pSourceLayer(sourceLayerPtr),
    m_pNodes(new VSimNode::map_t),
    m_pTriangles(new VSimTriangle::list_t)
{
}

const VLayer::ptr& VLayerFromLayerBuilder::build()
{
    m_nodeMaxId = m_pSourceLayer->getNodeMaxId() - m_pSourceLayer->getNodeMinId()
            + m_nodeStartId;
    m_triangleMaxId = m_pSourceLayer->getTriangleMaxId() - m_pSourceLayer->getTriangleMinId()
            + m_triangleStartId;
    createNodes();
    createTriangles();
    m_pLayer.reset(new VLayer(m_layerId, m_pNodes, m_pTriangles, m_pMaterial));
    m_pLayer->setMinMaxIds(m_nodeStartId, m_nodeMaxId, m_triangleStartId, m_triangleMaxId);
    m_pLayer->setName(m_layerName);
    return m_pLayer;
}

void VLayerFromLayerBuilder::createNodes()
{
    const VSimNode::map_ptr p_sourceNodes{m_pSourceLayer->getNodes()};
    const uint nodeIdDiff{m_nodeStartId - m_pSourceLayer->getNodeMinId()};
    m_pNodes->reserve(p_sourceNodes->size());
    for (auto &sourceNodePair : *p_sourceNodes)
    {
        uint destId{sourceNodePair.first + nodeIdDiff};
        const QVector3D &pos{sourceNodePair.second->getPosition()};
        VSimNode::ptr newNode(new VSimNode(destId, pos, m_pMaterial));
        m_pNodes->insert(std::make_pair(destId, newNode));
    }
    for (auto &sourceNodePair : *p_sourceNodes)
    {
        uint destId{sourceNodePair.first + nodeIdDiff};
        std::vector<uint> sourceNeighboursIds;
        sourceNodePair.second->getNeighboursId(sourceNeighboursIds, VSimNode::CURRENT);
        VSimNode::ptr destNode{m_pNodes->find(destId)->second};
        for (auto sourceNeighbourId : sourceNeighboursIds)
        {
            uint destNeighbourId{sourceNeighbourId + nodeIdDiff};
            VSimNode::ptr destNeighbourNode{m_pNodes->find(destNeighbourId)->second};
            destNode->addNeighbour(destNeighbourNode);
        }
    }
}

void VLayerFromLayerBuilder::createTriangles()
{
    const VSimTriangle::list_ptr p_sourceTriangles{m_pSourceLayer->getTriangles()};
    const uint nodeIdDiff{m_nodeStartId - m_pSourceLayer->getNodeMinId()};
    const uint triangleIdDiff{m_triangleStartId - m_pSourceLayer->getTriangleMinId()};
    for (auto &sourceTriangle : *p_sourceTriangles)
    {
        std::vector<uint> sourceVerticesIds = sourceTriangle->getVerticesId();
        std::vector<VSimNode::ptr> destNodePtrs;
        destNodePtrs.reserve(sourceVerticesIds.size());
        for(auto sourceId : sourceVerticesIds)
        {
            uint destId{sourceId + nodeIdDiff};
            auto nodePairIter = m_pNodes->find(destId);
            if (nodePairIter != m_pNodes->end())
            {
                destNodePtrs.push_back(nodePairIter->second);
            }
        }
        if(destNodePtrs.size() >= VSimTriangle::VERTICES_NUMBER)
        {
            uint triangleId{sourceTriangle->getId() + triangleIdDiff};
            VSimTriangle::ptr newTriangle(new VSimTriangle(triangleId, m_pMaterial,
                                          destNodePtrs.at(0), destNodePtrs.at(1),
                                          destNodePtrs.at(2)));
            m_pTriangles->push_back(newTriangle);
        }
    }
}
