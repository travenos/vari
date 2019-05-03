/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <gmsh/Gmsh.h>
#include <gmsh/GModel.h>

#include <QPolygonF>
#include <QDir>

#include "VLayerManualBuilder.h"
#include "VLayerFromGmeshBuilder.h"
#include "../structures/VExceptions.h"

#ifndef MAXFLOAT
#define MAXFLOAT FLT_MAX
#endif

/**
 * VLayerManualBuilder implementation
 */

const QString VLayerManualBuilder::TEMP_FILE_NAME("temp_vari_mesh.gmsh");
const double VLayerManualBuilder::GMSH_VERSION = 2.2;
const double VLayerManualBuilder::DEVIATION_PART = 0.1;

VLayerManualBuilder::VLayerManualBuilder(const QPolygonF &polygon,
                                         const VCloth &material,
                                         double characteristicLength) :
    VLayerAbstractBuilder(material),
    m_polygon(polygon),
    m_characteristicLength(characteristicLength)
{

}

const VLayer::ptr& VLayerManualBuilder::build()
{
    QString tempFileName = QDir::cleanPath(QDir::tempPath() + QDir::separator() + TEMP_FILE_NAME);
    try
    {
        exportToFile(m_polygon, tempFileName, m_characteristicLength);
    }
    catch(VExportException &)
    {
        throw VImportException();
    }
    VLayerFromGmeshBuilder gmshLoader(tempFileName, *m_pMaterial, M);
    gmshLoader.setLayerId(m_layerId);
    gmshLoader.setNodeStartId(m_nodeStartId);
    gmshLoader.setTriangleStartId(m_triangleStartId);
    QFile tempFile(tempFileName);
    try
    {
        m_pLayer = gmshLoader.build();
        m_nodeMaxId = gmshLoader.getNodeMaxId();
        m_triangleMaxId = gmshLoader.getTriangleMaxId();
    }
    catch(VImportException &e)
    {
        tempFile.remove();
        throw e;
    }
    tempFile.remove();
    m_pLayer->setName(m_layerName);
    return m_pLayer;
}

void VLayerManualBuilder::exportToFile(const QPolygonF &polygon, const QString &filename,
                                       double characteristicLength)
{
    const double lc = MAXFLOAT;
    double lengthMin = characteristicLength * (1 - DEVIATION_PART);
    double lengthMax = characteristicLength * (1 + DEVIATION_PART);
    GmshInitialize();
    GmshSetOption("Mesh", "CharacteristicLengthMin", lengthMin);
    GmshSetOption("Mesh", "CharacteristicLengthMax", lengthMax);

    GModel *m = new GModel;
    m->setFactory("Gmsh");

    // Add boundary vertices and edges of outline polygon
    std::vector<std::vector<GEdge *> > edges(1);
    edges[0].reserve(polygon.size());

    GVertex *v = nullptr, *vOld = nullptr, *v0 = nullptr;
    for (int i = 0; i < polygon.size(); ++i)
    {
        const QPointF &point = polygon[i];
        vOld = v;
        v = m->addVertex(point.x(), point.y(), 0., lc);
        if ( i != 0)
            edges[0].push_back(m->addLine(vOld, v));
        else
            v0 = v;
    }
    if (v != v0)
        edges[0].push_back(m->addLine(v, v0));

    // Create surface
    m->addPlanarFace(edges);
    // Create mesh
    m->mesh(2);
    int result = m->writeMSH(filename.toStdString(), GMSH_VERSION);
    delete m;

    GmshFinalize();
    if (!result)
        throw VExportException();
}
