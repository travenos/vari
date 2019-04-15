/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VMODELIMPORT_H
#define _VMODELIMPORT_H

#include <unordered_map>

#include "VModelImportExport.h"
#include "VLayersProcessor.h"

class VTable;
struct VInjectionVacuum;

class QXmlStreamReader;

class VModelImport : public VModelImportExport
{
public:
    VModelImport();
    const std::shared_ptr<VSimulationInfo>& getInfo() const;
    const std::shared_ptr<VSimulationParameters>& getSimulationParameters() const;
    const std::shared_ptr<VTable>& getTable() const;
    const std::shared_ptr<VInjectionVacuum>& getInjectionVacuum() const;
    const VLayersProcessor::ptr& getLayersProcessor() const;
    bool getUseTableParameters() const;
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
    void loadParameters(QXmlStreamReader &xmlReader);
    void loadPaused(QXmlStreamReader& xmlReader);
    void loadTimeLimit(QXmlStreamReader& xmlReader);
    void loadLayers(QXmlStreamReader &xmlReader);
    void loadConnections(QXmlStreamReader& xmlReader);

    void loadResin(QXmlStreamReader& xmlReader, VSimulationParameters &param);

    void loadTable(QXmlStreamReader& xmlReader);
    void readInjectionVacuum(QXmlStreamReader& xmlReader, VInjectionVacuum &injectionVacuum);
    void loadUseInjectionVacuum(QXmlStreamReader& xmlReader);

    void loadLayer(QXmlStreamReader& xmlReader, std::deque<VLayer::ptr> &layers);
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

    std::shared_ptr<VSimulationInfo> m_pInfo;
    std::shared_ptr<VSimulationParameters> m_pParam;
    std::shared_ptr<VTable> m_pTable;
    std::shared_ptr<VInjectionVacuum> m_pInjectionVacuum;
    VLayersProcessor::ptr m_pLayersProcessor;
    bool m_useTableParameters;
    bool m_paused;
    bool m_timeLimited;

    VSimNode::map_t m_allNodes;
};

#endif // _VMODELIMPORT_H
