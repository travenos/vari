/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VMODELIMPORTEXPORT_H
#define _VMODELIMPORTEXPORT_H

#include <QString>

class VModelImportExport
{
protected:
    struct VXmlInfoTags
    {
        const QString NAME{QStringLiteral("Info")};
        const QString SIM_TIME{QStringLiteral("simTime")};
        const QString REAL_TIME{QStringLiteral("realTime")};
        const QString REALTIME_FACTOR{QStringLiteral("realtimeFactor")};
        const QString FILLED_PERCENT{QStringLiteral("filledPercent")};
        const QString AVERAGE_PRESSURE{QStringLiteral("averagePressure")};
        const QString ITERATION{QStringLiteral("iteration")};
    };
    struct VXmlParamTags
    {
        struct VXmlResinTags
        {
            const QString NAME{QStringLiteral("Resin")};
            const QString TEMP_COEF{QStringLiteral("tempcoef")};
            const QString DEFAULT_VISCOSITY{QStringLiteral("defaultViscosity")};
            const QString MATERIAL_NAME{QStringLiteral("name")};
        };

        const QString NAME{QStringLiteral("Param")};
        const QString INJECTION_DIAMETER{QStringLiteral("injectionDiameter")};
        const QString VACUUM_DIAMETER{QStringLiteral("vacuumDiameter")};
        const QString TEMPERATURE{QStringLiteral("temperature")};
        const QString INJECTION_PRESSURE{QStringLiteral("injectionPressure")};
        const QString VACUUM_PRESSURE{QStringLiteral("vacuumPressure")};
        const QString Q{QStringLiteral("q")};
        const QString R{QStringLiteral("r")};
        const QString S{QStringLiteral("s")};
        const QString AVERAGE_CELL_DISTANCE{QStringLiteral("averageCellDistance")};
        const QString AVERAGE_PERMEABILITY{QStringLiteral("averagePermeability")};
        const QString NUMBER_OF_FULL_NODES{QStringLiteral("numberOfFullNodes")};
    };
    struct VXmlPausedTags
    {
        const QString NAME{QStringLiteral("Paused")};
    };
    struct VXmlLayersTags
    {
        struct VXmlLayerTags
        {
            struct VXmlClothTags
            {
                const QString NAME{QStringLiteral("Cloth")};
                const QString CAVITY_HEIGHT{QStringLiteral("cavityHeight")};
                const QString PERMEABILITY{QStringLiteral("permeability")};
                const QString POROSITY{QStringLiteral("porosity")};
                const QString BASE_COLOR{QStringLiteral("baseColor")};
                const QString MATERIAL_NAME{QStringLiteral("name")};
            };
            struct VXmlNodesTags
            {
                struct VXmlNodeTags
                {
                    const QString NAME{QStringLiteral("Node")};
                    const QString ID{QStringLiteral("id")};
                    const QString PRESSURE{QStringLiteral("press")};
                    const QString NEW_PRESSURE{QStringLiteral("nPress")};
                    const QString ROLE{QStringLiteral("role")};
                };

                const QString NAME{QStringLiteral("Nodes")};
            };
            struct VXmlTrianglesTags
            {
                struct VXmlTriangleTags
                {
                    const QString NAME{QStringLiteral("Trngl")};
                    const QString ID{QStringLiteral("id")};
                    const QString COLOR{QStringLiteral("color")};
                };

                const QString NAME{QStringLiteral("Triangles")};
            };

            const QString NAME{QStringLiteral("Layer")};
            const QString NUMBER{QStringLiteral("number")};
            const QString IS_ENABLED{QStringLiteral("isEnabled")};
            const QString IS_VISIBLE{QStringLiteral("isVisible")};
        };

        const QString NAME{QStringLiteral("Layers")};
    };
    struct VXmlConnectionsTags
    {
        struct VXmlConnectionTags
        {
            const QString NAME{QStringLiteral("Cnt")};
            const QString ID{QStringLiteral("id")};

        };

        const QString NAME{QStringLiteral("Connections")};
    };

};

#endif // _VMODELIMPORTEXPORT_H
