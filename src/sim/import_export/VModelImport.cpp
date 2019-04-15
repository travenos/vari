/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif
#include <QFile>
#include <QXmlStreamReader>
#include <QVariant>

#include "VModelImport.h"
#include "structures/VExceptions.h"
#include "structures/VTable.h"

#define READ_ELEMENTS(body) \
    while (!(xmlReader.isEndElement() && !xmlReader.name().compare(tags._NAME)) && !xmlReader.atEnd()) \
    { \
        if(xmlReader.isStartElement()) \
        { \
            body \
        } \
        xmlReader.readNext(); \
    }

VModelImport::VModelImport():
    m_pInfo(new VSimulationInfo),
    m_pParam(new VSimulationParameters),
    m_pTable(new VTable),
    m_pInjectionVacuum(new VInjectionVacuum),
    m_pLayersProcessor(new VLayersProcessor),
    m_useTableParameters(true),
    m_paused(false),
    m_timeLimited(false)
{
    m_pInjectionVacuum->injectionDiameter = m_pParam->getInjectionDiameter();
    m_pInjectionVacuum->vacuumDiameter = m_pParam->getVacuumDiameter();
}

const std::shared_ptr<VSimulationInfo>& VModelImport::getInfo() const
{
    return m_pInfo;
}

const std::shared_ptr<VSimulationParameters> &VModelImport::getSimulationParameters() const
{
    return m_pParam;
}

const std::shared_ptr<VTable>& VModelImport::getTable() const
{
    return m_pTable;
}

const std::shared_ptr<VInjectionVacuum>& VModelImport::getInjectionVacuum() const
{
    return m_pInjectionVacuum;
}

const VLayersProcessor::ptr& VModelImport::getLayersProcessor() const
{
    return m_pLayersProcessor;
}

bool VModelImport::getUseTableParameters() const
{
    return m_useTableParameters;
}

bool VModelImport::getPaused() const
{
    return m_paused;
}

bool VModelImport::getTimeLimited() const
{
    return m_timeLimited;
}

void VModelImport::loadFromFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QFile::Text))
        throw VImportException();
    m_allNodes.clear();
    QXmlStreamReader xmlReader(&file);
    xmlReader.readNext();
    while(!xmlReader.atEnd())
    {        
        if(xmlReader.isStartElement())
        {
            #ifdef DEBUG_MODE
                qDebug() << xmlReader.name();
            #endif
            if (!xmlReader.name().compare(_xINFO_TAGS._NAME))
                loadInfo(xmlReader);
            else if (!xmlReader.name().compare(_xPARAM_TAGS._NAME))
                loadParameters(xmlReader);
            else if (!xmlReader.name().compare(_xTABLE_TAGS._NAME))
                loadTable(xmlReader);
            else if (!xmlReader.name().compare(_xUSE_INJECTION_VACUUM_TAGS._NAME))
                loadUseInjectionVacuum(xmlReader);
            else if (!xmlReader.name().compare(_xPAUSED_TAGS._NAME))
                loadPaused(xmlReader);
            else if (!xmlReader.name().compare(_xTIMELIMIT_TAGS._NAME))
                loadTimeLimit(xmlReader);
            else if (!xmlReader.name().compare(_xLAYERS_TAGS._NAME))
                loadLayers(xmlReader);
            else if (!xmlReader.name().compare(_xCONNECTIONS_TAGS._NAME))
                loadConnections(xmlReader);
        }
        xmlReader.readNext();
    }
    if(xmlReader.hasError())
    {
        #ifdef DEBUG_MODE
            qDebug() << xmlReader.errorString();
        #endif
        file.close();
        throw VImportException();
    }
    file.close();
    m_pLayersProcessor->resetNodesVolumes();
}

void VModelImport::loadInfo(QXmlStreamReader &xmlReader)
{
    auto &tags = _xINFO_TAGS;
    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes())
    {
        if (!attr.name().compare(tags.AVERAGE_PRESSURE))
            m_pInfo->averagePressure = attr.value().toDouble();
        else if (!attr.name().compare(tags.FILLED_PERCENT))
            m_pInfo->filledPercent = attr.value().toDouble();
        else if (!attr.name().compare(tags.REALTIME_FACTOR))
            m_pInfo->realtimeFactor = attr.value().toDouble();
        else if (!attr.name().compare(tags.REAL_TIME))
            m_pInfo->realTime = attr.value().toDouble();
        else if (!attr.name().compare(tags.SIM_TIME))
            m_pInfo->simTime = attr.value().toDouble();
        else if (!attr.name().compare(tags.ITERATION))
            m_pInfo->iteration = attr.value().toInt();
    }
}

void VModelImport::loadParameters(QXmlStreamReader& xmlReader)
{
    auto &tags = _xPARAM_TAGS;
    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes())
    {
        if (!attr.name().compare(tags.AVERAGE_CELL_DISTANCE))
            m_pParam->setAverageCellDistance(attr.value().toDouble());
        else if (!attr.name().compare(tags.AVERAGE_PERMEABILITY))
            m_pParam->setAveragePermeability(attr.value().toDouble());
        else if (!attr.name().compare(tags.INJECTION_DIAMETER))
            m_pParam->setInjectionDiameter(attr.value().toFloat());
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
            m_pParam->setVacuumDiameter(attr.value().toFloat());
        else if (!attr.name().compare(tags.VACUUM_PRESSURE))
            m_pParam->setVacuumPressure(attr.value().toDouble());
        else if (!attr.name().compare(tags.NUMBER_OF_FULL_NODES))
            m_pParam->setNumberOfFullNodes(attr.value().toInt());
    }
    READ_ELEMENTS
    (
        if (!xmlReader.name().compare(tags._xRESIN_TAGS._NAME))
            loadResin(xmlReader, *m_pParam);
    );
}

void VModelImport::loadResin(QXmlStreamReader &xmlReader, VSimulationParameters &param)
{
    auto &tags = _xPARAM_TAGS._xRESIN_TAGS;
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
    param.setResin(resin);
}


void VModelImport::loadTable(QXmlStreamReader& xmlReader)
{
    auto &tags = _xTABLE_TAGS;
    READ_ELEMENTS
    (
        if (!xmlReader.name().compare(tags._xINJECTION_VACUUM_TAGS._NAME))
        {
            VInjectionVacuum injectionVacuum;
            readInjectionVacuum(xmlReader, injectionVacuum);
            m_pTable->setInjectionVacuum(injectionVacuum);
        }
        if (!xmlReader.name().compare(tags.SIZE))
        {
            QString tableSizeStr = xmlReader.readElementText();
            std::vector<float> tableSizeVector;
            makeVector(tableSizeStr, tableSizeVector);
            if(tableSizeVector.size() > 1)
                m_pTable->setSize(tableSizeVector.at(0), tableSizeVector.at(1));
        }
    );
}

void VModelImport::readInjectionVacuum(QXmlStreamReader& xmlReader, VInjectionVacuum &injectionVacuum)
{
    auto &tags = _xINJECTION_VACUUM_TAGS;
    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes())
    {
        if (!attr.name().compare(tags.INJECTION_DIAMETER))
            injectionVacuum.injectionDiameter = attr.value().toFloat();
        if (!attr.name().compare(tags.VACUUM_DIAMETER))
            injectionVacuum.vacuumDiameter = attr.value().toFloat();
    }
    READ_ELEMENTS
    (
        bool isInjection = !xmlReader.name().compare(tags.INJECTION_COORDS);
        bool isVacuum = !xmlReader.name().compare(tags.VACUUM_COORDS);
        if (isInjection || isVacuum)
        {
            QString coordsStr = xmlReader.readElementText();
            std::vector<float> coordsVector;
            makeVector(coordsStr, coordsVector);
            if(coordsVector.size() > 1)
            {
                if (isInjection)
                {
                    injectionVacuum.injectionCoords.setX(coordsVector.at(0));
                    injectionVacuum.injectionCoords.setY(coordsVector.at(1));
                }
                else
                {
                    injectionVacuum.vacuumCoords.setX(coordsVector.at(0));
                    injectionVacuum.vacuumCoords.setY(coordsVector.at(1));
                }
            }
        }
    );
}

void VModelImport::loadUseInjectionVacuum(QXmlStreamReader& xmlReader)
{
    auto &tags = _xUSE_INJECTION_VACUUM_TAGS;
    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes())
    {
        if (!attr.name().compare(tags.USE_TABLE_PARAMETERS))
            m_useTableParameters = static_cast<bool>(attr.value().toInt());
    }
    READ_ELEMENTS
    (
        if (!xmlReader.name().compare(tags._xINJECTION_VACUUM_TAGS._NAME))
        {
            readInjectionVacuum(xmlReader, *m_pInjectionVacuum);
        }
    );
}

void VModelImport::loadPaused(QXmlStreamReader &xmlReader)
{
    m_paused = static_cast<bool>(xmlReader.readElementText().toInt());
}

void VModelImport::loadTimeLimit(QXmlStreamReader &xmlReader)
{
    auto &tags = _xTIMELIMIT_TAGS;
    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes())
    {
        if (!attr.name().compare(tags.DURATION))
            m_pParam->setTimeLimit(attr.value().toDouble());
    }
    m_timeLimited = static_cast<bool>(xmlReader.readElementText().toInt());
}

void VModelImport::loadLayers(QXmlStreamReader& xmlReader)
{
    auto &tags = _xLAYERS_TAGS;
    std::deque<VLayer::ptr> layers;
    READ_ELEMENTS
    (
        if (!xmlReader.name().compare(tags._xLAYER_TAGS._NAME))
            loadLayer(xmlReader, layers);
    );
    m_pLayersProcessor->setLayers(layers);
}

void VModelImport::loadLayer(QXmlStreamReader& xmlReader, std::deque<VLayer::ptr>& layers)
{
    auto &tags = _xLAYERS_TAGS._xLAYER_TAGS;
    bool isEnabled=true, isVisible=true;
    uint nodeMinId=0, nodeMaxId=0, triangleMinId=0, triangleMaxId=0;
    uint nodesNumber=0, trianglesNumber=0;
    VSimNode::map_ptr nodes(new VSimNode::map_t);
    VSimTriangle::list_ptr triangles(new VSimTriangle::list_t);
    std::vector<VTriangleInfo> trianglesInfos;
    VCloth::ptr material(new VCloth);
    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes())
    {
        if (!attr.name().compare(tags.IS_ENABLED))
            isEnabled = static_cast<bool>(attr.value().toInt());
        else if (!attr.name().compare(tags.IS_VISIBLE))            
            isVisible = static_cast<bool>(attr.value().toInt());
        else if (!attr.name().compare(tags.NODE_MIN_ID))
            nodeMinId = attr.value().toUInt();
        else if (!attr.name().compare(tags.NODE_MAX_ID))
            nodeMaxId = attr.value().toUInt();
        else if (!attr.name().compare(tags.TRIANGLE_MIN_ID))
            triangleMinId = attr.value().toUInt();
        else if (!attr.name().compare(tags.TRIANGLE_MAX_ID))
            triangleMaxId = attr.value().toUInt();
        else if (!attr.name().compare(tags.NUMBER_OF_NODES))
            nodesNumber = attr.value().toUInt();
        else if (!attr.name().compare(tags.NUMBER_OF_TRIANGLES))
            trianglesNumber = attr.value().toUInt();
    }
    nodes->reserve(nodesNumber);
    m_allNodes.reserve(m_allNodes.size() + nodesNumber);
    trianglesInfos.reserve(trianglesNumber);
    READ_ELEMENTS
    (
        if (!xmlReader.name().compare(tags._xCLOTH_TAGS._NAME))
            loadCloth(xmlReader, material);
        if (!xmlReader.name().compare(tags._xNODES_TAGS._NAME))
            loadNodes(xmlReader, material, nodes);
        if (!xmlReader.name().compare(tags._xTRIANGLES_TAGS._NAME))
            loadTrianglesInfos(xmlReader, trianglesInfos);
    );
    createTriangles(trianglesInfos, material, triangles);
    VLayer::ptr layer(new VLayer(layers.size(), nodes, triangles, material, false));
    layer->markActive(isEnabled);
    layer->setVisible(isVisible);
    layer->setMinMaxIds(nodeMinId, nodeMaxId, triangleMinId, triangleMaxId);
    layers.push_back(layer);
}

void VModelImport::loadCloth(QXmlStreamReader& xmlReader, VCloth::ptr &material)
{
    auto &tags = _xLAYERS_TAGS._xLAYER_TAGS._xCLOTH_TAGS;
    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes())
    {
        if (!attr.name().compare(tags.BASE_COLOR))
            material->baseColor.setNamedColor(attr.value().toString());
        else if (!attr.name().compare(tags.CAVITY_HEIGHT))
            material->cavityHeight = attr.value().toDouble();
        else if (!attr.name().compare(tags.MATERIAL_NAME))
            material->name = attr.value().toString();
        else if (!attr.name().compare(tags.PERMEABILITY))
            material->permeability = attr.value().toDouble();
        else if (!attr.name().compare(tags.POROSITY))
            material->porosity = attr.value().toDouble();
    }
}

void VModelImport::loadNodes(QXmlStreamReader& xmlReader,
                             const VCloth::const_ptr &material, const VSimNode::map_ptr &nodes)
{
    auto &tags = _xLAYERS_TAGS._xLAYER_TAGS._xNODES_TAGS;
    READ_ELEMENTS
    (
        if (!xmlReader.name().compare(tags._xNODE_TAGS._NAME))
            loadNode(xmlReader, material, nodes);
    );
}

void VModelImport::loadNode(QXmlStreamReader& xmlReader, const VCloth::const_ptr &material,
                            const VSimNode::map_ptr &nodes)
{
    auto &tags = _xLAYERS_TAGS._xLAYER_TAGS._xNODES_TAGS._xNODE_TAGS;
    const size_t DIMS = 3;
    uint id = 0;
    double pressure=0;
    VSimNode::VNodeRole role = VSimNode::NORMAL;
    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes())
    {
        if (!attr.name().compare(tags.ID))
            id = attr.value().toUInt();
        else if (!attr.name().compare(tags.PRESSURE))
            pressure = attr.value().toDouble();
        else if (!attr.name().compare(tags.ROLE))
            role = static_cast<VSimNode::VNodeRole>(attr.value().toUInt());
    }
    QString positionStr = xmlReader.readElementText();
    std::vector<float> positionVect;
    positionVect.reserve(DIMS);
    makeVector(positionStr, positionVect);
    if (positionVect.size() == DIMS)
    {
        QVector3D pos(positionVect[0], positionVect[1], positionVect[2]);
        VSimNode::ptr nodePtr;
        nodePtr.reset(new VSimNode(id, pos, material,
                                   m_pParam->getInjectionPressure(),
                                   m_pParam->getVacuumPressure(),
                                   pressure, role));
        if (m_allNodes.insert({id, nodePtr}).second)
            nodes->insert({id, nodePtr});
        else
            throw VImportException();
    }
    else
        throw VImportException();
}

void VModelImport::loadTrianglesInfos(QXmlStreamReader& xmlReader,
                                      std::vector<VTriangleInfo> &trianglesInfos)
{
    auto &tags = _xLAYERS_TAGS._xLAYER_TAGS._xTRIANGLES_TAGS;
    READ_ELEMENTS
    (
        if (!xmlReader.name().compare(tags._xTRIANGLE_TAGS._NAME))
            loadTriangleInfo(xmlReader, trianglesInfos);
    );
}

void VModelImport::loadTriangleInfo(QXmlStreamReader& xmlReader,
                                    std::vector<VTriangleInfo> &trianglesInfos)
{
    auto &tags = _xLAYERS_TAGS._xLAYER_TAGS._xTRIANGLES_TAGS._xTRIANGLE_TAGS;
    VTriangleInfo triangleInfo;
    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes())
    {
        if (!attr.name().compare(tags.ID))
            triangleInfo.id = attr.value().toUInt();
        else if (!attr.name().compare(tags.COLOR))
            triangleInfo.color.setNamedColor(attr.value().toString());
    }
    QString idStr = xmlReader.readElementText();
    std::vector<uint> idVect;
    idVect.reserve(VSimTriangle::VERTICES_NUMBER);
    makeVector(idStr, idVect);
    if (idVect.size() == VSimTriangle::VERTICES_NUMBER)
    {
        triangleInfo.nodeIds = idVect;
        trianglesInfos.push_back(triangleInfo);
    }
    else
        throw VImportException();
}

void VModelImport::createTriangles(const std::vector<VTriangleInfo> &trianglesInfos,
                                   const VCloth::const_ptr &material,
                                   const VSimTriangle::list_ptr &triangles)
{
    for (auto &trInf : trianglesInfos)
    {
        VSimNode::const_ptr nodes[VSimTriangle::VERTICES_NUMBER];
        std::unordered_map<uint, VSimNode::ptr>::const_iterator nodeRef;
        for (uint i = 0; i < VSimTriangle::VERTICES_NUMBER; ++i)
        {
            nodeRef = m_allNodes.find(trInf.nodeIds[i]);
            if (nodeRef != m_allNodes.end())
                nodes[i] = nodeRef->second;
            else
                throw VImportException();
        }
        VSimTriangle::ptr triangle(new VSimTriangle(trInf.id, material,
                                                    nodes[0], nodes[1], nodes[2],
                                                    trInf.color));
        triangles->push_back(triangle);
    }
}

void VModelImport::loadConnections(QXmlStreamReader &xmlReader)
{
    auto &tags = _xCONNECTIONS_TAGS;
    READ_ELEMENTS
    (
        if (!xmlReader.name().compare(tags._xCONNECTION_TAGS._NAME))
            loadConnection(xmlReader);
    );
}

void VModelImport::loadConnection(QXmlStreamReader& xmlReader)
{
    auto &tags = _xCONNECTIONS_TAGS._xCONNECTION_TAGS;
    uint id = 0;
    bool idGot = false;
    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes())
    {
        if (!attr.name().compare(tags.ID))
        {
            id = attr.value().toUInt();
            idGot = true;
        }
    }
    if (idGot)
    {
        READ_ELEMENTS
        (
            if (!xmlReader.name().compare(tags._xCURRENT_TAGS._NAME))
            {
                QString nbIdStr = xmlReader.readElementText();
                    createConnection(nbIdStr, id);
            }
        );
    }
    else
        throw VImportException();
}

void VModelImport::createConnection(const QString &nbIdStr, uint id)
{
    std::vector<uint> nbIdVect;
    makeVector(nbIdStr, nbIdVect);
    auto nodeRef = m_allNodes.find(id);
    if (nodeRef != m_allNodes.end())
    {
        VSimNode::ptr node = nodeRef->second;
        for (auto nbId : nbIdVect)
        {
            auto nbNodeRef = m_allNodes.find(nbId);
            if (nbNodeRef != m_allNodes.end())
            {
                VSimNode::ptr nbNode = nbNodeRef->second;
                node->addNeighbour(nbNode.get());
            }
            else
                throw VImportException();
        }
    }
    else
        throw VImportException();
}

template <typename T>
void VModelImport::makeVector(const QString &numbersStr, std::vector<T> &numbersVect)
{
    numbersVect.clear();
    QStringList numberStrings = numbersStr.split(' ');
    numbersVect.reserve(static_cast<size_t>(numberStrings.size()));
    foreach (const QString &str, numberStrings)
    {
        T number = qvariant_cast<T>(str);
        numbersVect.push_back(number);
    }
}
