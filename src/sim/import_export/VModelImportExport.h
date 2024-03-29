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
            const QString VISC_TEMP_COEF{QStringLiteral("viscTempcoef")};
            const QString DEFAULT_VISCOSITY{QStringLiteral("defaultViscosity")};
            const QString LIFETIME_TEMP_COEF{"lifetimeTempcoef"};
            const QString DEFAULT_LIFETIME{"lifetime"};
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
    struct VXmlInjectionVacuumTags
    {
        VXmlInjectionVacuumTags() {}
        const QString _NAME{QStringLiteral("InjectionVacuum")};
        const QString INJECTION_COORDS{QStringLiteral("injectionCoords")};
        const QString VACUUM_COORDS{QStringLiteral("vacuumCoords")};
        const QString INJECTION_DIAMETER{QStringLiteral("injectionDiameter")};
        const QString VACUUM_DIAMETER{QStringLiteral("vacuumDiameter")};
    };
    struct VXmlUseInjectionVacuumTags
    {
        VXmlUseInjectionVacuumTags() {}
        const QString _NAME{QStringLiteral("UseInjectionVacuum")};
        const QString USE_TABLE_PARAMETERS{QStringLiteral("useTable")};

        const VXmlInjectionVacuumTags _xINJECTION_VACUUM_TAGS;
    };
    struct VXmlPausedTags
    {
        VXmlPausedTags() {}
        const QString _NAME{QStringLiteral("Paused")};
    };
    struct VXmlTimeLimitTags
    {
        VXmlTimeLimitTags() {}
        const QString _NAME{QStringLiteral("TimeLimit")};
        const QString DURATION{QStringLiteral("Duration")};
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
                const QString DIAG_PERMEABILITY{QStringLiteral("permeability")};
                const QString X_PERMEABILITY{QStringLiteral("xPermeability")};
                const QString Y_PERMEABILITY{QStringLiteral("yPermeability")};
                const QString POROSITY{QStringLiteral("porosity")};
                const QString ANGLE{QStringLiteral("angle")};
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
            const QString LAYER_NAME{QStringLiteral("name")};

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
            const QString _NAME{QStringLiteral("Cnt")};
            const QString ID{QStringLiteral("id")};

            const VXmlCurLayerConnectionTags _xCURRENT_TAGS;
        };

        const QString _NAME{QStringLiteral("Connections")};

        const VXmlConnectionTags _xCONNECTION_TAGS;
    };
    const QString HEAD_TAG_NAME{QStringLiteral("Resources")};

    const VXmlInfoTags _xINFO_TAGS;
    const VXmlParamTags _xPARAM_TAGS;
    const VXmlInjectionVacuumTags _xINJECTION_VACUUM_TAGS;
    const VXmlUseInjectionVacuumTags _xUSE_INJECTION_VACUUM_TAGS;
    const VXmlPausedTags _xPAUSED_TAGS;
    const VXmlTimeLimitTags _xTIMELIMIT_TAGS;
    const VXmlLayersTags _xLAYERS_TAGS;
    const VXmlConnectionsTags _xCONNECTIONS_TAGS;
};

#endif // _VMODELIMPORTEXPORT_H
