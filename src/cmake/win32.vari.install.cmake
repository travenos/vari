set(SQL_DIR "${PROJECT_SOURCE_DIR}/../share/vari/sql")
if(MSVC_IDE)
    set(CONFIG_DIRECTORY_RELEASE ${EXECUTABLE_OUTPUT_PATH}/Release)
else()
    set(CONFIG_DIRECTORY_RELEASE ${EXECUTABLE_OUTPUT_PATH})
endif()

find_file(DLL_MSVCR msvcr120.dll)
find_file(DLL_MSVCP msvcp120.dll)

set(CONFIG_DLL
    ${DLL_MSVCR}
    ${DLL_MSVCP}
    )

file(COPY ${CONFIG_DLL} DESTINATION ${CONFIG_DIRECTORY_RELEASE})
file(COPY ${SQL_DIR} DESTINATION ${CONFIG_DIRECTORY_RELEASE})

if(NOT DEFINED APP_NAME)
    set(APP_NAME ${PROJECT_NAME})
endif()
set(ISS_FILE ${CONFIG_DIRECTORY_RELEASE}/win32.vari.install.iss)
configure_file(${PROJECT_SOURCE_DIR}/cmake/win32.vari.install.iss.cmake ${ISS_FILE})
message(STATUS "Inno Setup script is saved to file: " ${ISS_FILE})
