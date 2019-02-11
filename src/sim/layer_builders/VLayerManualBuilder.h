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
                        double characteristicLength);
    
    const VLayer::ptr &build() override;

    static void exportToFile(const QPolygonF &polygon, const QString &filename,
                             double characteristicLength);

protected:
    static const QString TEMP_FILE_NAME;
    static const double GMSH_VERSION;
    static const double DEVIATION_PART;
    QPolygonF m_polygon;
    double m_characteristicLength;
};

#endif //_VLAYERMANUALBUILDER_H
