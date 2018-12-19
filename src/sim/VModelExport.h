/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VMODELEXPORT_H
#define _VMODELEXPORT_H

#include "VModelImportExport.h"
#include "VLayersProcessor.h"

class QXmlStreamWriter;

class VModelExport : public VModelImportExport
{
public:
    VModelExport() = delete;
    VModelExport(const VSimulationInfo &info,
                 const VSimulationParametres::const_ptr &param,
                 const VLayersProcessor::const_ptr &layersProcessor,
                 bool paused = false);
    void setInfo(const VSimulationInfo &info);
    void setSimulationParametres(const VSimulationParametres::const_ptr &param);
    void setLayersProcessor(const VLayersProcessor::const_ptr &layersProcesor);
    void setPaused(bool paused);
    void saveToFile(const QString &filename);

protected:
    VSimulationInfo m_info;
    VSimulationParametres::const_ptr m_pParam;
    VLayersProcessor::const_ptr m_pLayersProcessor;
    bool m_paused;

private:
    void saveInfo(QXmlStreamWriter &xmlWriter);

    void saveParametres(QXmlStreamWriter &xmlWriter);

    void saveResin(QXmlStreamWriter &xmlWriter, const VResin &resin);

    void savePaused(QXmlStreamWriter &xmlWriter);

    void saveLayers(QXmlStreamWriter &xmlWriter);

    void saveLayer(QXmlStreamWriter &xmlWriter, uint layer);

    void saveCloth(QXmlStreamWriter &xmlWriter, const VCloth::const_ptr &cloth);

    void saveNodes(QXmlStreamWriter &xmlWriter,
                                     const VSimNode::const_vector_ptr &nodes);

    void saveNode(QXmlStreamWriter &xmlWriter,
                                     const VSimNode::const_ptr &node);

    void saveTriangles(QXmlStreamWriter &xmlWriter,
                                     const VSimTriangle::const_vector_ptr &triangles);

    void saveTriangle(QXmlStreamWriter &xmlWriter,
                                     const VSimTriangle::const_ptr &triangle);

    void saveConnections(QXmlStreamWriter &xmlWriter);

    void saveConnection(QXmlStreamWriter &xmlWriter,
                                     const VSimNode::const_ptr &node);

    QString createString(const QVector3D &vect) const;

    template <typename T>
    QString createString(const std::vector<T> &vect) const;
};

#endif // _VMODELEXPORT_H