SET(SQL_DIR "${PROJECT_SOURCE_DIR}/../share/vari/sql")
IF(MSVC_IDE)
    SET(CONFIG_DIRECTORY_RELEASE ${EXECUTABLE_OUTPUT_PATH}/Release)
ELSE()
    SET(CONFIG_DIRECTORY_RELEASE ${EXECUTABLE_OUTPUT_PATH})
ENDIF()

FIND_FILE(DLL_MSVCR msvcr120.dll)
FIND_FILE(DLL_MSVCP msvcp120.dll)

SET(CONFIG_DLL
    ${DLL_MSVCR}
    ${DLL_MSVCP}
    )

FILE(COPY ${CONFIG_DLL} DESTINATION ${CONFIG_DIRECTORY_RELEASE})
FILE(COPY ${SQL_DIR} DESTINATION ${CONFIG_DIRECTORY_RELEASE})
