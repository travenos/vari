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
#include "structures/VExceptions.h"

VModelExport::VModelExport(const VSimulationInfo &info,
                           const VSimulationParameters &param,
                           const VTable &table,
                           const VInjectionVacuum &injectionVacuum,
                           const VLayersProcessor::const_ptr &layersProcessor,
                           bool useTableParameters,
                           bool paused,
                           bool timeLimited):
    m_info(info),
    m_param(param),
    m_table(table),
    m_injectionVacuum(injectionVacuum),
    m_pLayersProcessor(layersProcessor),
    m_useTableParameters(useTableParameters),
    m_paused(paused),
    m_timeLimited(timeLimited)
{}

void VModelExport::setInfo(const VSimulationInfo &info)
{
    m_info= info;
}

void VModelExport::setSimulationParameters(const VSimulationParameters &param)
{
    m_param = param;
}

void VModelExport::setLayersProcessor(const VLayersProcessor::const_ptr &layersProcesor)
{
    m_pLayersProcessor = layersProcesor;
}

void VModelExport::setPaused(bool paused)
{
    m_paused = paused;
}

void VModelExport::setTimeLimited(bool timeLimited)
{
    m_timeLimited = timeLimited;
}

void VModelExport::saveToFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        throw VExportException();
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement(HEAD_TAG_NAME);
    saveInfo(xmlWriter);
    saveParameters(xmlWriter);
    saveTable(xmlWriter);
    saveUseInjectionVacuum(xmlWriter);
    savePaused(xmlWriter);
    saveTimeLimit(xmlWriter);
    saveLayers(xmlWriter);
    saveConnections(xmlWriter);
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    file.close();
}

void VModelExport::saveInfo(QXmlStreamWriter &xmlWriter)
{
    auto &tags = _xINFO_TAGS;
    xmlWriter.writeStartElement(tags._NAME);
    xmlWriter.writeAttribute(tags.SIM_TIME, QString::number(m_info.simTime));
    xmlWriter.writeAttribute(tags.REAL_TIME, QString::number(m_info.realTime));
    xmlWriter.writeAttribute(tags.REALTIME_FACTOR, QString::number(m_info.realtimeFactor));
    xmlWriter.writeAttribute(tags.FILLED_PERCENT, QString::number(m_info.filledPercent));
    xmlWriter.writeAttribute(tags.AVERAGE_PRESSURE, QString::number(m_info.averagePressure));
    xmlWriter.writeAttribute(tags.ITERATION, QString::number(m_info.iteration));
    xmlWriter.writeEndElement();
}

void VModelExport::saveParameters(QXmlStreamWriter &xmlWriter)
{
    auto &tags = _xPARAM_TAGS;
    xmlWriter.writeStartElement(tags._NAME);
    xmlWriter.writeAttribute(tags.INJECTION_DIAMETER, QString::number(m_param.getInjectionDiameter()));
    xmlWriter.writeAttribute(tags.VACUUM_DIAMETER, QString::number(m_param.getVacuumDiameter()));
    xmlWriter.writeAttribute(tags.TEMPERATURE, QString::number(m_param.getTemperature()));
    xmlWriter.writeAttribute(tags.INJECTION_PRESSURE, QString::number(m_param.getInjectionPressure()));
    xmlWriter.writeAttribute(tags.VACUUM_PRESSURE, QString::number(m_param.getVacuumPressure()));
    xmlWriter.writeAttribute(tags.Q, QString::number(m_param.getQ()));
    xmlWriter.writeAttribute(tags.R, QString::number(m_param.getR()));
    xmlWriter.writeAttribute(tags.S, QString::number(m_param.getS()));
    xmlWriter.writeAttribute(tags.AVERAGE_CELL_DISTANCE, QString::number(m_param.getAverageCellDistance()));;
    xmlWriter.writeAttribute(tags.AVERAGE_PERMEABILITY, QString::number(m_param.getAveragePermeability()));
    xmlWriter.writeAttribute(tags.NUMBER_OF_FULL_NODES, QString::number(m_param.getNumberOfFullNodes()));
    saveResin(xmlWriter, m_param.getResin());
    xmlWriter.writeEndElement();
}

void VModelExport::saveResin(QXmlStreamWriter &xmlWriter, const VResin &resin)
{
    auto &tags = _xPARAM_TAGS._xRESIN_TAGS;
    xmlWriter.writeStartElement(tags._NAME);
    xmlWriter.writeAttribute(tags.VISC_TEMP_COEF, QString::number(resin.viscTempcoef));
    xmlWriter.writeAttribute(tags.DEFAULT_VISCOSITY, QString::number(resin.defaultViscosity));
    xmlWriter.writeAttribute(tags.LIFETIME_TEMP_COEF, QString::number(resin.lifetimeTempcoef));
    xmlWriter.writeAttribute(tags.DEFAULT_LIFETIME, QString::number(resin.defaultLifetime));
    xmlWriter.writeAttribute(tags.MATERIAL_NAME, resin.name);
    xmlWriter.writeEndElement();
}

void VModelExport::saveTable(QXmlStreamWriter& xmlWriter)
{
    auto &tags = _xTABLE_TAGS;
    xmlWriter.writeStartElement(tags._NAME);

    xmlWriter.writeStartElement(tags.SIZE);
    xmlWriter.writeCharacters(createString(m_table.getSize()));
    xmlWriter.writeEndElement();

    writeInjectionVacuum(xmlWriter, m_table.getInjectionVacuum());
    xmlWriter.writeEndElement();
}

void VModelExport::writeInjectionVacuum(QXmlStreamWriter& xmlWriter,
                                        const VInjectionVacuum &injectionVacuum)
{
    auto &tags = _xINJECTION_VACUUM_TAGS;
    xmlWriter.writeStartElement(tags._NAME);
    xmlWriter.writeAttribute(tags.INJECTION_DIAMETER, QString::number(injectionVacuum.injectionDiameter));
    xmlWriter.writeAttribute(tags.VACUUM_DIAMETER, QString::number(injectionVacuum.vacuumDiameter));

    xmlWriter.writeStartElement(tags.INJECTION_COORDS);
    xmlWriter.writeCharacters(createString(injectionVacuum.injectionCoords));
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement(tags.VACUUM_COORDS);
    xmlWriter.writeCharacters(createString(injectionVacuum.vacuumCoords));
    xmlWriter.writeEndElement();

    xmlWriter.writeEndElement();
}

void VModelExport::saveUseInjectionVacuum(QXmlStreamWriter& xmlWriter)
{
    auto &tags = _xUSE_INJECTION_VACUUM_TAGS;
    xmlWriter.writeStartElement(tags._NAME);
    xmlWriter.writeAttribute(tags.USE_TABLE_PARAMETERS, QString::number(m_useTableParameters));
    writeInjectionVacuum(xmlWriter, m_injectionVacuum);
    xmlWriter.writeEndElement();
}

void VModelExport::savePaused(QXmlStreamWriter &xmlWriter)
{
    auto &tags = _xPAUSED_TAGS;
    xmlWriter.writeStartElement(tags._NAME);
    xmlWriter.writeCharacters(QString::number(m_paused));
    xmlWriter.writeEndElement();
}

void VModelExport::saveTimeLimit(QXmlStreamWriter &xmlWriter)
{
    auto &tags = _xTIMELIMIT_TAGS;
    xmlWriter.writeStartElement(tags._NAME);
    xmlWriter.writeAttribute(tags.DURATION, QString::number(m_param.getTimeLimit()));
    xmlWriter.writeCharacters(QString::number(m_timeLimited));
    xmlWriter.writeEndElement();
}

void VModelExport::saveLayers(QXmlStreamWriter &xmlWriter)
{
    auto &tags = _xLAYERS_TAGS;
    xmlWriter.writeStartElement(tags._NAME);
    for (uint i = 0; i < m_pLayersProcessor->getLayersNumber(); ++i)
        saveLayer(xmlWriter, i);
    xmlWriter.writeEndElement();
}

void VModelExport::saveLayer(QXmlStreamWriter &xmlWriter, uint layer)
{
    auto &tags = _xLAYERS_TAGS._xLAYER_TAGS;
    xmlWriter.writeStartElement(tags._NAME);
    xmlWriter.writeAttribute(tags.IS_ENABLED,
                             QString::number(m_pLayersProcessor->isLayerEnabled(layer)));
    xmlWriter.writeAttribute(tags.IS_VISIBLE,
                             QString::number(m_pLayersProcessor->isLayerVisible(layer)));
    xmlWriter.writeAttribute(tags.NODE_MIN_ID,
                             QString::number(m_pLayersProcessor->getNodeMinId(layer)));
    xmlWriter.writeAttribute(tags.NODE_MAX_ID,
                             QString::number(m_pLayersProcessor->getNodeMaxId(layer)));
    xmlWriter.writeAttribute(tags.TRIANGLE_MIN_ID,
                             QString::number(m_pLayersProcessor->getTriangleMinId(layer)));
    xmlWriter.writeAttribute(tags.TRIANGLE_MAX_ID,
                             QString::number(m_pLayersProcessor->getTriangleMaxId(layer)));
    xmlWriter.writeAttribute(tags.NUMBER_OF_NODES,
                             QString::number(m_pLayersProcessor->getLayerNodesNumber(layer)));
    xmlWriter.writeAttribute(tags.NUMBER_OF_TRIANGLES,
                             QString::number(m_pLayersProcessor->getLayerTrianglesNumber(layer)));
    xmlWriter.writeAttribute(tags.LAYER_NAME, m_pLayersProcessor->getLayerName(layer));
    saveCloth(xmlWriter, m_pLayersProcessor->getMaterial(layer));
    saveNodes(xmlWriter, m_pLayersProcessor->getLayerNodes(layer));
    saveTriangles(xmlWriter, m_pLayersProcessor->getLayerTriangles(layer));
    xmlWriter.writeEndElement();
}

void VModelExport::saveCloth(QXmlStreamWriter &xmlWriter, const VCloth::const_ptr &cloth)
{
    auto &tags = _xLAYERS_TAGS._xLAYER_TAGS._xCLOTH_TAGS;
    xmlWriter.writeStartElement(tags._NAME);
    xmlWriter.writeAttribute(tags.CAVITY_HEIGHT, QString::number(cloth->cavityHeight));
    xmlWriter.writeAttribute(tags.PERMEABILITY, QString::number(cloth->permeability));
    xmlWriter.writeAttribute(tags.POROSITY, QString::number(cloth->porosity));
    xmlWriter.writeAttribute(tags.BASE_COLOR, cloth->baseColor.name());
    xmlWriter.writeAttribute(tags.MATERIAL_NAME, cloth->name);
    xmlWriter.writeEndElement();
}

void VModelExport::saveNodes(QXmlStreamWriter &xmlWriter,
                                 const VSimNode::const_map_ptr &nodes)
{
    auto &tags = _xLAYERS_TAGS._xLAYER_TAGS._xNODES_TAGS;
    xmlWriter.writeStartElement(tags._NAME);
    for (auto &node : *nodes)
        saveNode(xmlWriter, node.second);
    xmlWriter.writeEndElement();
}

void VModelExport::saveNode(QXmlStreamWriter &xmlWriter,
                                 const VSimNode::const_ptr &node)
{
    auto &tags = _xLAYERS_TAGS._xLAYER_TAGS._xNODES_TAGS._xNODE_TAGS;
    xmlWriter.writeStartElement(tags._NAME);
    xmlWriter.writeAttribute(tags.ID, QString::number(node->getId()));
    xmlWriter.writeAttribute(tags.PRESSURE, QString::number(node->getPressure()));
    xmlWriter.writeAttribute(tags.ROLE, QString::number(node->getRole()));
    xmlWriter.writeCharacters(createString(node->getPosition()));
    xmlWriter.writeEndElement();
}

void VModelExport::saveTriangles(QXmlStreamWriter &xmlWriter,
                                 const VSimTriangle::const_list_ptr &triangles)
{
    auto &tags = _xLAYERS_TAGS._xLAYER_TAGS._xTRIANGLES_TAGS;
    xmlWriter.writeStartElement(tags._NAME);
    for (auto &triangle : *triangles)
        saveTriangle(xmlWriter, triangle);
    xmlWriter.writeEndElement();
}

void VModelExport::saveTriangle(QXmlStreamWriter &xmlWriter,
                                 const VSimTriangle::const_ptr &triangle)
{
    auto &tags = _xLAYERS_TAGS._xLAYER_TAGS._xTRIANGLES_TAGS._xTRIANGLE_TAGS;
    xmlWriter.writeStartElement(tags._NAME);
    xmlWriter.writeAttribute(tags.ID, QString::number(triangle->getId()));
    xmlWriter.writeAttribute(tags.COLOR, triangle->getColor().name());
    xmlWriter.writeCharacters(createString(triangle->getVerticesId()));
    xmlWriter.writeEndElement();
}

void VModelExport::saveConnections(QXmlStreamWriter &xmlWriter)
{
    auto &tags = _xCONNECTIONS_TAGS;
    xmlWriter.writeStartElement(tags._NAME);
    for (uint i = 0; i < m_pLayersProcessor->getLayersNumber(); ++i)
    {
        VSimNode::const_map_ptr nodes = m_pLayersProcessor->getLayerNodes(i);
        for (auto &node : *nodes)
            saveConnection(xmlWriter, node.second);
    }
    xmlWriter.writeEndElement();
}

void VModelExport::saveConnection(QXmlStreamWriter &xmlWriter,
                                 const VSimNode::const_ptr &node)
{
    auto &tags = _xCONNECTIONS_TAGS._xCONNECTION_TAGS;
    xmlWriter.writeStartElement(tags._NAME);
    xmlWriter.writeAttribute(tags.ID, QString::number(node->getId()));
    saveConnectionCurrentLayer(xmlWriter, node);
    xmlWriter.writeEndElement();
}

void VModelExport::saveConnectionCurrentLayer(QXmlStreamWriter &xmlWriter,
                                 const VSimNode::const_ptr &node)
{
    std::vector<uint> connections;
    node->getNeighboursId(connections, VSimNode::CURRENT);
    if (connections.size() > 0)
    {
        auto &tags = _xCONNECTIONS_TAGS._xCONNECTION_TAGS._xCURRENT_TAGS;
        xmlWriter.writeStartElement(tags._NAME);
        xmlWriter.writeCharacters(createString(connections));
        xmlWriter.writeEndElement();
    }
}

QString VModelExport::createString(const QVector2D &vect) const
{
    return _createString(vect, 2u);
}

QString VModelExport::createString(const QVector3D &vect) const
{
    return _createString(vect, 3u);
}

template <typename T>
QString VModelExport::createString(const std::vector<T> &vect) const
{
    return _createString(vect, vect.size());
}

template <typename T>
QString VModelExport::_createString(const T &vect, size_t size) const
{
    QString str;
    for (size_t i = 0; i < size; ++i)
    {
        if (i > 0)
            str.append(" ");
        str.append(QString::number(vect[i]));
    }
    return str;
}
