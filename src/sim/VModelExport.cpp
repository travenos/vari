/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif

#include <QFile>
#include <QXmlStreamWriter>

#include "VModelExport.h"
#include "core/VExceptions.h"

VModelExport::VModelExport(const VSimulationInfo &info,
                           const VSimulationParametres::const_ptr &param,
                           const VLayersProcessor::const_ptr &layersProcessor,
                           bool paused):
    m_info(info),
    m_pParam(param),
    m_pLayersProcessor(layersProcessor),
    m_paused(paused)
{}

void VModelExport::setInfo(const VSimulationInfo &info)
{
    m_info= info;
}

void VModelExport::setSimulationParametres(const VSimulationParametres::const_ptr &param)
{
    m_pParam = param;
}

void VModelExport::setLayersProcessor(const VLayersProcessor::const_ptr &layersProcesor)
{
    m_pLayersProcessor = layersProcesor;
}

void VModelExport::setPaused(bool paused)
{
    m_paused = paused;
}

void VModelExport::saveToFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        throw VExportException();
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    saveInfo(xmlWriter);
    saveParametres(xmlWriter);
    savePaused(xmlWriter);
    saveLayers(xmlWriter);
    saveConnections(xmlWriter);
    xmlWriter.writeEndDocument();
    file.close();
}

void VModelExport::saveInfo(QXmlStreamWriter &xmlWriter)
{
    VXmlInfoTags tags;
    xmlWriter.writeStartElement(tags.NAME);
    xmlWriter.writeAttribute(tags.SIM_TIME, QString::number(m_info.simTime));
    xmlWriter.writeAttribute(tags.REAL_TIME, QString::number(m_info.realTime));
    xmlWriter.writeAttribute(tags.REALTIME_FACTOR, QString::number(m_info.realtimeFactor));
    xmlWriter.writeAttribute(tags.FILLED_PERCENT, QString::number(m_info.filledPercent));
    xmlWriter.writeAttribute(tags.AVERAGE_PRESSURE, QString::number(m_info.averagePressure));
    xmlWriter.writeAttribute(tags.ITERATION, QString::number(m_info.iteration));
    xmlWriter.writeEndElement();
}

void VModelExport::saveParametres(QXmlStreamWriter &xmlWriter)
{
    VXmlParamTags tags;
    xmlWriter.writeStartElement(tags.NAME);
    xmlWriter.writeAttribute(tags.INJECTION_DIAMETER, QString::number(m_pParam->getInjectionDiameter()));
    xmlWriter.writeAttribute(tags.VACUUM_DIAMETER, QString::number(m_pParam->getVacuumDiameter()));
    xmlWriter.writeAttribute(tags.TEMPERATURE, QString::number(m_pParam->getTemperature()));
    xmlWriter.writeAttribute(tags.INJECTION_PRESSURE, QString::number(m_pParam->getInjectionPressure()));
    xmlWriter.writeAttribute(tags.VACUUM_PRESSURE, QString::number(m_pParam->getVacuumPressure()));
    xmlWriter.writeAttribute(tags.Q, QString::number(m_pParam->getQ()));
    xmlWriter.writeAttribute(tags.R, QString::number(m_pParam->getR()));
    xmlWriter.writeAttribute(tags.S, QString::number(m_pParam->getS()));
    xmlWriter.writeAttribute(tags.AVERAGE_CELL_DISTANCE, QString::number(m_pParam->getAverageCellDistance()));;
    xmlWriter.writeAttribute(tags.AVERAGE_PERMEABILITY, QString::number(m_pParam->getAveragePermeability()));
    xmlWriter.writeAttribute(tags.NUMBER_OF_FULL_NODES, QString::number(m_pParam->getNumberOfFullNodes()));
    saveResin(xmlWriter, m_pParam->getResin());
    xmlWriter.writeEndElement();
}

void VModelExport::saveResin(QXmlStreamWriter &xmlWriter, const VResin &resin)
{
    VXmlParamTags::VXmlResinTags tags;
    xmlWriter.writeStartElement(tags.NAME);
    xmlWriter.writeAttribute(tags.TEMP_COEF, QString::number(resin.tempcoef));
    xmlWriter.writeAttribute(tags.DEFAULT_VISCOSITY, QString::number(resin.defaultViscosity));
    xmlWriter.writeAttribute(tags.MATERIAL_NAME, resin.name);
    xmlWriter.writeEndElement();
}

void VModelExport::savePaused(QXmlStreamWriter &xmlWriter)
{
    VXmlPausedTags tags;
    xmlWriter.writeStartElement(tags.NAME);
    xmlWriter.writeCharacters(QString::number(m_paused));
    xmlWriter.writeEndElement();
}

void VModelExport::saveLayers(QXmlStreamWriter &xmlWriter)
{
    VXmlLayersTags tags;
    xmlWriter.writeStartElement(tags.NAME);
    for (uint i = 0; i < m_pLayersProcessor->getLayersNumber(); ++i)
        saveLayer(xmlWriter, i);
    xmlWriter.writeEndElement();
}

void VModelExport::saveLayer(QXmlStreamWriter &xmlWriter, uint layer)
{
    VXmlLayersTags::VXmlLayerTags tags;
    xmlWriter.writeStartElement(tags.NAME);
    xmlWriter.writeAttribute(tags.NUMBER, QString::number(layer));
    xmlWriter.writeAttribute(tags.IS_ENABLED,
                             QString::number(m_pLayersProcessor->isLayerEnabled(layer)));
    xmlWriter.writeAttribute(tags.IS_VISIBLE,
                             QString::number(m_pLayersProcessor->isLayerVisible(layer)));
    saveCloth(xmlWriter, m_pLayersProcessor->getMaterial(layer));
    saveNodes(xmlWriter, m_pLayersProcessor->getLayerNodes(layer));
    saveTriangles(xmlWriter, m_pLayersProcessor->getLayerTriangles(layer));
    xmlWriter.writeEndElement();
}

void VModelExport::saveCloth(QXmlStreamWriter &xmlWriter, const VCloth::const_ptr &cloth)
{
    VXmlLayersTags::VXmlLayerTags::VXmlClothTags tags;
    xmlWriter.writeStartElement(tags.NAME);
    xmlWriter.writeAttribute(tags.CAVITY_HEIGHT, QString::number(cloth->cavityHeight));
    xmlWriter.writeAttribute(tags.PERMEABILITY, QString::number(cloth->permeability));
    xmlWriter.writeAttribute(tags.POROSITY, QString::number(cloth->porosity));
    xmlWriter.writeAttribute(tags.BASE_COLOR, cloth->baseColor.name());
    xmlWriter.writeAttribute(tags.MATERIAL_NAME, cloth->name);
    xmlWriter.writeEndElement();
}

void VModelExport::saveNodes(QXmlStreamWriter &xmlWriter,
                                 const VSimNode::const_vector_ptr &nodes)
{
    VXmlLayersTags::VXmlLayerTags::VXmlNodesTags tags;
    xmlWriter.writeStartElement(tags.NAME);
    for (auto &node : *nodes)
        saveNode(xmlWriter, node);
    xmlWriter.writeEndElement();
}

void VModelExport::saveNode(QXmlStreamWriter &xmlWriter,
                                 const VSimNode::const_ptr &node)
{
    VXmlLayersTags::VXmlLayerTags::VXmlNodesTags::VXmlNodeTags tags;
    xmlWriter.writeStartElement(tags.NAME);
    xmlWriter.writeAttribute(tags.ID, QString::number(node->getId()));
    xmlWriter.writeAttribute(tags.PRESSURE, QString::number(node->getPressure()));
    xmlWriter.writeAttribute(tags.NEW_PRESSURE, QString::number(node->getNewPressure()));
    xmlWriter.writeCharacters(createString(node->getPosition()));
    xmlWriter.writeEndElement();
}

void VModelExport::saveTriangles(QXmlStreamWriter &xmlWriter,
                                 const VSimTriangle::const_vector_ptr &triangles)
{
    VXmlLayersTags::VXmlLayerTags::VXmlTrianglesTags tags;
    xmlWriter.writeStartElement(tags.NAME);
    for (auto &triangle : *triangles)
        saveTriangle(xmlWriter, triangle);
    xmlWriter.writeEndElement();
}

void VModelExport::saveTriangle(QXmlStreamWriter &xmlWriter,
                                 const VSimTriangle::const_ptr &triangle)
{
    VXmlLayersTags::VXmlLayerTags::VXmlTrianglesTags::VXmlTriangleTags tags;
    xmlWriter.writeStartElement(tags.NAME);
    xmlWriter.writeAttribute(tags.ID, QString::number(triangle->getId()));
    xmlWriter.writeAttribute(tags.COLOR, triangle->getColor().name());
    xmlWriter.writeCharacters(createString(triangle->getVerticesId()));
    xmlWriter.writeEndElement();
}

void VModelExport::saveConnections(QXmlStreamWriter &xmlWriter)
{
    VXmlConnectionsTags tags;
    xmlWriter.writeStartElement(tags.NAME);
    for (uint i = 0; i < m_pLayersProcessor->getLayersNumber(); ++i)
    {
        VSimNode::const_vector_ptr nodes = m_pLayersProcessor->getLayerNodes(i);
        for (auto &node : *nodes)
            saveConnection(xmlWriter, node);
    }
    xmlWriter.writeEndElement();
}

void VModelExport::saveConnection(QXmlStreamWriter &xmlWriter,
                                 const VSimNode::const_ptr &node)
{
    VXmlConnectionsTags::VXmlConnectionTags tags;
    xmlWriter.writeStartElement(tags.NAME);
    xmlWriter.writeAttribute(tags.ID, QString::number(node->getId()));
    std::vector<uint> connections;
    node->getNeighboursId(connections);
    xmlWriter.writeCharacters(createString(connections));
    xmlWriter.writeEndElement();
}

QString VModelExport::createString(const QVector3D &vect) const
{
    QString str;
    for (int i=0; i < 3 ; ++i)
    {
        str.append(" ");
        str.append(QString::number(vect[i]));
    }
    return str;
}

template <typename T>
QString VModelExport::createString(const std::vector<T> &vect) const
{
    QString str;
    for (auto num : vect)
    {
        str.append(" ");
        str.append(QString::number(num));
    }
    return str;
}
