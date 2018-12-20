/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VMODELIMPORTEXPORT_H
#define _VMODELIMPORTEXPORT_H

#include <QString>

class VModelImportExport
{
public:
    VModelImportExport() {}
protected:
    struct VXmlInfoTags
    {
        VXmlInfoTags() {}
        const QString _NAME{QStringLiteral("Info")};
        const QString SIM_TIME{QStringLiteral("simTime")};
        const QString REAL_TIME{QStringLiteral("realTime")};
        const QString REALTIME_FACTOR{QStringLiteral("realtimeFactor")};
        const QString FILLED_PERCENT{QStringLiteral("filledPercent")};
        const QString AVERAGE_PRESSURE{QStringLiteral("averagePressure")};
        const QString ITERATION{QStringLiteral("iteration")};
    };
    struct VXmlParamTags
    {
        VXmlParamTags() {}
        struct VXmlResinTags
        {
            VXmlResinTags() {}
            const QString _NAME{QStringLiteral("Resin")};
            const QString TEMP_COEF{QStringLiteral("tempcoef")};
            const QString DEFAULT_VISCOSITY{QStringLiteral("defaultViscosity")};
            const QString MATERIAL_NAME{QStringLiteral("name")};
        };

        const QString _NAME{QStringLiteral("Param")};
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

        const VXmlResinTags _xRESIN_TAGS;
    };
    struct VXmlPausedTags
    {
        VXmlPausedTags() {}
        const QString _NAME{QStringLiteral("Paused")};
    };
    struct VXmlLayersTags
    {
        VXmlLayersTags() {}
        struct VXmlLayerTags
        {
            VXmlLayerTags() {}
            struct VXmlClothTags
            {
                VXmlClothTags() {}
                const QString _NAME{QStringLiteral("Cloth")};
                const QString CAVITY_HEIGHT{QStringLiteral("cavityHeight")};
                const QString PERMEABILITY{QStringLiteral("permeability")};
                const QString POROSITY{QStringLiteral("porosity")};
                const QString BASE_COLOR{QStringLiteral("baseColor")};
                const QString MATERIAL_NAME{QStringLiteral("name")};
            };
            struct VXmlNodesTags
            {
                VXmlNodesTags() {}
                struct VXmlNodeTags
                {
                    VXmlNodeTags() {}
                    const QString _NAME{QStringLiteral("Node")};
                    const QString ID{QStringLiteral("id")};
                    const QString PRESSURE{QStringLiteral("press")};
                    const QString NEW_PRESSURE{QStringLiteral("nPress")};
                    const QString ROLE{QStringLiteral("role")};
                };

                const QString _NAME{QStringLiteral("Nodes")};

                const VXmlNodeTags _xNODE_TAGS;
            };
            struct VXmlTrianglesTags
            {
                VXmlTrianglesTags() {}
                struct VXmlTriangleTags
                {
                    VXmlTriangleTags() {}
                    const QString _NAME{QStringLiteral("Trngl")};
                    const QString ID{QStringLiteral("id")};
                    const QString COLOR{QStringLiteral("color")};
                };

                const QString _NAME{QStringLiteral("Triangles")};

                const VXmlTriangleTags _xTRIANGLE_TAGS;
            };

            const QString _NAME{QStringLiteral("Layer")};
            const QString IS_ENABLED{QStringLiteral("isEnabled")};
            const QString IS_VISIBLE{QStringLiteral("isVisible")};
            const QString NODE_MIN_ID{QStringLiteral("midNodeId")};
            const QString NODE_MAX_ID{QStringLiteral("maxNodeId")};
            const QString TRIANGLE_MIN_ID{QStringLiteral("minTriangId")};
            const QString TRIANGLE_MAX_ID{QStringLiteral("maxTriangId")};
            const QString NUMBER_OF_NODES{QStringLiteral("nodesCount")};
            const QString NUMBER_OF_TRIANGLES{QStringLiteral("triangCount")};

            const VXmlClothTags _xCLOTH_TAGS;
            const VXmlNodesTags _xNODES_TAGS;
            const VXmlTrianglesTags _xTRIANGLES_TAGS;
        };

        const QString _NAME{QStringLiteral("Layers")};

        const VXmlLayerTags _xLAYER_TAGS;
    };
    struct VXmlConnectionsTags
    {
        VXmlConnectionsTags() {}
        struct VXmlConnectionTags
        {
            VXmlConnectionTags() {}
            struct VXmlCurLayerConnectionTags
            {
                VXmlCurLayerConnectionTags() {}
                const QString _NAME{QStringLiteral("Cur")};
            };
            struct VXmlPrevLayerConnectionTags
            {
                VXmlPrevLayerConnectionTags() {}
                const QString _NAME{QStringLiteral("Prev")};
            };
            const QString _NAME{QStringLiteral("Cnt")};
            const QString ID{QStringLiteral("id")};

            const VXmlCurLayerConnectionTags _xCURRENT_TAGS;
            const VXmlPrevLayerConnectionTags _xPREVIOUS_TAGS;
        };

        const QString _NAME{QStringLiteral("Connections")};

        const VXmlConnectionTags _xCONNECTION_TAGS;
    };
    const QString HEAD_TAG_NAME{QStringLiteral("Resources")};

    const VXmlInfoTags _xINFO_TAGS;
    const VXmlParamTags _xPARAM_TAGS;
    const VXmlPausedTags _xPAUSED_TAGS;
    const VXmlLayersTags _xLAYERS_TAGS;
    const VXmlConnectionsTags _xCONNECTIONS_TAGS;
};

#endif // _VMODELIMPORTEXPORT_H
