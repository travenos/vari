/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VMODELEXPORT_H
#define _VMODELEXPORT_H

#include "VModelImportExport.h"
#include "VLayersProcessor.h"
#include "structures/VTable.h"

class QXmlStreamWriter;

class VModelExport : public VModelImportExport
{
public:
    VModelExport() = delete;
    VModelExport(const VSimulationInfo &info,
                 const VSimulationParameters &param,
                 const VTable &table,
                 const VInjectionVacuum &injectionVacuum,
                 const VLayersProcessor::const_ptr &layersProcessor,
                 bool useTableParameters = true,
                 bool paused = false,
                 bool timeLimited = false);
    void setInfo(const VSimulationInfo &info);
    void setSimulationParameters(const VSimulationParameters &param);
    void setTable(const VTable &table);
    void setInjectionVacuum(const VInjectionVacuum &injectionVacuum);
    void setLayersProcessor(const VLayersProcessor::const_ptr &layersProcesor);
    void setUseTableParameters(bool useTableParameters);
    void setPaused(bool paused);
    void setTimeLimited(bool timeLimited);
    void saveToFile(const QString &filename);

private:
    void saveInfo(QXmlStreamWriter &xmlWriter);

    void saveParameters(QXmlStreamWriter &xmlWriter);

    void saveResin(QXmlStreamWriter &xmlWriter, const VResin &resin);

    void saveTable(QXmlStreamWriter& xmlWriter);
    void writeInjectionVacuum(QXmlStreamWriter& xmlWriter, const VInjectionVacuum &injectionVacuum);
    void saveUseInjectionVacuum(QXmlStreamWriter& xmlWriter);

    void savePaused(QXmlStreamWriter &xmlWriter);

    void saveTimeLimit(QXmlStreamWriter &xmlWriter);

    void saveLayers(QXmlStreamWriter &xmlWriter);

    void saveLayer(QXmlStreamWriter &xmlWriter, uint layer);

    void saveCloth(QXmlStreamWriter &xmlWriter, const VCloth::const_ptr &cloth);

    void saveNodes(QXmlStreamWriter &xmlWriter,
                                     const VSimNode::const_map_ptr &nodes);

    void saveNode(QXmlStreamWriter &xmlWriter,
                                     const VSimNode::const_ptr &node);

    void saveTriangles(QXmlStreamWriter &xmlWriter,
                                     const VSimTriangle::const_list_ptr &triangles);

    void saveTriangle(QXmlStreamWriter &xmlWriter,
                                     const VSimTriangle::const_ptr &triangle);

    void saveConnections(QXmlStreamWriter &xmlWriter);

    void saveConnection(QXmlStreamWriter &xmlWriter,
                                     const VSimNode::const_ptr &node);

    void saveConnectionCurrentLayer(QXmlStreamWriter &xmlWriter,
                                     const VSimNode::const_ptr &node);

    QString createString(const QVector2D &vect) const;
    QString createString(const QVector3D &vect) const;

    template <typename T>
    QString createString(const std::vector<T> &vect) const;

    template <typename T>
    QString _createString(const T &vect, size_t size) const;

    VSimulationInfo m_info;
    VSimulationParameters m_param;
    VTable m_table;
    VInjectionVacuum m_injectionVacuum;
    VLayersProcessor::const_ptr m_pLayersProcessor;
    bool m_useTableParameters;
    bool m_paused;
    bool m_timeLimited;
};

#endif // _VMODELEXPORT_H
