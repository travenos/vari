/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VMODELIMPORT_H
#define _VMODELIMPORT_H

#include <unordered_map>

#include "VModelImportExport.h"
#include "VLayersProcessor.h"

class QXmlStreamReader;

class VModelImport : public VModelImportExport
{
public:
    VModelImport(VSimulationParametres::const_ptr paramPointer);
    const VSimulationInfo& getInfo() const;
    const VSimulationParametres &getSimulationParametres() const;
    const VLayersProcessor::ptr& getLayersProcessor() const;
    bool getPaused() const;
    void loadFromFile(const QString &filename);

private:
    struct VTriangleInfo
    {
        VTriangleInfo() {}
        uint id;
        std::vector<uint> nodeIds;
        QColor color;
    };

    void loadInfo(QXmlStreamReader& xmlReader);
    void loadParametres(QXmlStreamReader &xmlReader);
    void loadPaused(QXmlStreamReader& xmlReader);
    void loadLayers(QXmlStreamReader &xmlReader);
    void loadConnections(QXmlStreamReader& xmlReader);

    void loadResin(QXmlStreamReader& xmlReader, VSimulationParametres &param);

    void loadLayer(QXmlStreamReader& xmlReader, std::vector<VLayer::ptr>& layers);
    void loadCloth(QXmlStreamReader& xmlReader, VCloth::ptr &material);
    void loadNodes(QXmlStreamReader& xmlReader, const VCloth::const_ptr &material,
                   VSimNode::vector_ptr &nodes);
    void loadTrianglesInfos(QXmlStreamReader& xmlReader, std::vector<VTriangleInfo> &trianglesInfos);
    void createTriangles(const std::vector<VTriangleInfo> &trianglesInfos,
                         const VCloth::const_ptr &material,
                         VSimTriangle::vector_ptr &triangles);

    void loadNode(QXmlStreamReader& xmlReader,
                  const VCloth::const_ptr &material, VSimNode::vector_ptr &nodes);
    void loadTriangleInfo(QXmlStreamReader& xmlReader, std::vector<VTriangleInfo> &trianglesInfos);

    void loadConnection(QXmlStreamReader& xmlReader);
    void createConnection(const QString &nbIdStr, uint id, bool current);

    template <typename T>
    void makeVector(const QString &numbersStr, std::vector<T> &numbersVect);

    VSimulationInfo m_info;
    VSimulationParametres m_param;
    VSimulationParametres::const_ptr m_pParamPtr;
    VLayersProcessor::ptr m_pLayersProcessor;
    bool m_paused;

    std::unordered_map<uint, VSimNode::ptr> m_allNodes;
};

#endif // _VMODELIMPORT_H
