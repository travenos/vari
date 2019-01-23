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
    VModelImport();
    const VSimulationInfo& getInfo() const;
    const VSimulationParametres &getSimulationParametres() const;
    const VLayersProcessor::ptr& getLayersProcessor() const;
    bool getPaused() const;
    bool getTimeLimited() const;
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
    void loadTimeLimit(QXmlStreamReader& xmlReader);
    void loadLayers(QXmlStreamReader &xmlReader);
    void loadConnections(QXmlStreamReader& xmlReader);

    void loadResin(QXmlStreamReader& xmlReader, VSimulationParametres &param);

    void loadLayer(QXmlStreamReader& xmlReader, std::vector<VLayer::ptr>& layers);
    void loadCloth(QXmlStreamReader& xmlReader, VCloth::ptr &material);
    void loadNodes(QXmlStreamReader& xmlReader, const VCloth::const_ptr &material,
                   const VSimNode::map_ptr &nodes);
    void loadTrianglesInfos(QXmlStreamReader& xmlReader, std::vector<VTriangleInfo> &trianglesInfos);
    void createTriangles(const std::vector<VTriangleInfo> &trianglesInfos,
                         const VCloth::const_ptr &material,
                         const VSimTriangle::list_ptr &triangles);

    void loadNode(QXmlStreamReader& xmlReader,
                  const VCloth::const_ptr &material, const VSimNode::map_ptr &nodes);
    void loadTriangleInfo(QXmlStreamReader& xmlReader, std::vector<VTriangleInfo> &trianglesInfos);

    void loadConnection(QXmlStreamReader& xmlReader);
    void createConnection(const QString &nbIdStr, uint id);

    template <typename T>
    void makeVector(const QString &numbersStr, std::vector<T> &numbersVect);

    VSimulationInfo m_info;
    VSimulationParametres m_param;
    VLayersProcessor::ptr m_pLayersProcessor;
    bool m_paused;
    bool m_timeLimited;

    VSimNode::map_t m_allNodes;
};

#endif // _VMODELIMPORT_H
