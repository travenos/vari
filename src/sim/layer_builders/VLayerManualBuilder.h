/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYERMANUALBUILDER_H
#define _VLAYERMANUALBUILDER_H

#include "VLayerAbstractBuilder.h"

class VLayerManualBuilder: public VLayerAbstractBuilder
{
public:
    VLayerManualBuilder(const QPolygonF &polygon,
                        const VCloth &material,
                        VUnit units=M);
    
    const VLayer::ptr &build() override;

    static void exportToFile(const QPolygonF &polygon, const QString &filename);

protected:
    static const QString TEMP_FILE_NAME;
    static const double GMSH_VERSION;
    QPolygonF m_polygon;
    VUnit m_units;
};

#endif //_VLAYERMANUALBUILDER_H
