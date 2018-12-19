/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <QFile>
#include <QXmlStreamReader>

#include "VModelImport.h"
#include "core/VExceptions.h"

VModelImport::VModelImport():
    m_pParam(new VSimulationParametres),
    m_pLayersProcessor(new VLayersProcessor)
{
}

const VSimulationInfo& VModelImport::getInfo() const
{
    return m_info;
}

const VSimulationParametres::ptr& VModelImport::getSimulationParametres() const
{
    return m_pParam;
}

const VLayersProcessor::ptr& VModelImport::getLayersProcessor() const
{
    return m_pLayersProcessor;
}

bool VModelImport::getPaused() const
{
    return m_paused;
}

void VModelImport::loadFromFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QFile::Text))
        throw VImportException();
    QXmlStreamReader xmlReader(&file);
    xmlReader.readNext();
    while(!xmlReader.atEnd())
    {
        if(xmlReader.isStartElement())
        {
            if (!xmlReader.name().compare(INFO_TAGS.NAME))
                loadInfo(xmlReader);
            else if (!xmlReader.name().compare(PARAM_TAGS.NAME))
                loadParametres(xmlReader);
            else if (!xmlReader.name().compare(PAUSED_TAGS.NAME))
                loadPaused(xmlReader);
            else if (!xmlReader.name().compare(LAYERS_TAGS.NAME))
                loadLayers(xmlReader);
            else if (!xmlReader.name().compare(CONNECTIONS_TAGS.NAME))
                loadConnections(xmlReader);
        }
        xmlReader.readNext();
    }
    file.close();
}

void VModelImport::loadInfo(QXmlStreamReader &xmlReader)
{
    auto &tags = INFO_TAGS;
    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes())
    {
        if (!attr.name().compare(tags.AVERAGE_PRESSURE))
            m_info.averagePressure = attr.value().toDouble();
        else if (!attr.name().compare(tags.FILLED_PERCENT))
            m_info.filledPercent = attr.value().toDouble();
        else if (!attr.name().compare(tags.REALTIME_FACTOR))
            m_info.realtimeFactor = attr.value().toDouble();
        else if (!attr.name().compare(tags.REAL_TIME))
            m_info.realTime = attr.value().toDouble();
        else if (!attr.name().compare(tags.SIM_TIME))
            m_info.simTime = attr.value().toDouble();
        else if (!attr.name().compare(tags.ITERATION))
            m_info.iteration = attr.value().toInt();
    }
}

void VModelImport::loadParametres(QXmlStreamReader& xmlReader)
{
    auto &tags = PARAM_TAGS;
    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes())
    {
        if (!attr.name().compare(tags.AVERAGE_CELL_DISTANCE))
            m_pParam->setAverageCellDistance(attr.value().toDouble());
        else if (!attr.name().compare(tags.AVERAGE_PERMEABILITY))
            m_pParam->setAveragePermeability(attr.value().toDouble());
        else if (!attr.name().compare(tags.INJECTION_DIAMETER))
            m_pParam->setInjectionDiameter(attr.value().toDouble());
        else if (!attr.name().compare(tags.INJECTION_PRESSURE))
            m_pParam->setInjectionPressure(attr.value().toDouble());
        else if (!attr.name().compare(tags.Q))
            m_pParam->setQ(attr.value().toDouble());
        else if (!attr.name().compare(tags.R))
            m_pParam->setR(attr.value().toDouble());
        else if (!attr.name().compare(tags.S))
            m_pParam->setS(attr.value().toDouble());
        else if (!attr.name().compare(tags.TEMPERATURE))
            m_pParam->setTemperature(attr.value().toDouble());
        else if (!attr.name().compare(tags.VACUUM_DIAMETER))
            m_pParam->setVacuumDiameter(attr.value().toDouble());
        else if (!attr.name().compare(tags.VACUUM_PRESSURE))
            m_pParam->setVacuumPressure(attr.value().toDouble());
        else if (!attr.name().compare(tags.NUMBER_OF_FULL_NODES))
            m_pParam->setNumberOfFullNodes(attr.value().toInt());
    }
    while (!xmlReader.isEndElement())
    {
        xmlReader.readNext();
        if(xmlReader.isStartElement())
        {
            if (!xmlReader.name().compare(tags.RESIN_TAGS.NAME))
                m_pParam->setResin(loadResin(xmlReader));
        }
    }
}

VResin VModelImport::loadResin(QXmlStreamReader &xmlReader) const
{
    auto &tags = PARAM_TAGS.RESIN_TAGS;
    VResin resin;
    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes())
    {
        if (!attr.name().compare(tags.DEFAULT_VISCOSITY))
            resin.defaultViscosity = attr.value().toDouble();
        else if (!attr.name().compare(tags.TEMP_COEF))
            resin.defaultViscosity = attr.value().toDouble();
        else if (!attr.name().compare(tags.MATERIAL_NAME))
            resin.name = attr.value().toString();
    }
    return resin;
}

void VModelImport::loadPaused(QXmlStreamReader &xmlReader)
{
    m_paused = static_cast<bool>(xmlReader.readElementText().toInt());
}

void VModelImport::loadLayers(QXmlStreamReader& xmlReader)
{

}

void VModelImport::loadConnections(QXmlStreamReader &xmlReader)
{

}
