# Project VARI
# @author Alexey Barashkov

find_package(Qt5Core    REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(HEADER_FILES
    VModelExport.h
    VModelImport.h
    VModelImportExport.h
    )

set(SOURCE_FILES
    VModelExport.cpp
    VModelImport.cpp
    )

ADD_MODULE(v_import_export
    ${HEADER_FILES}
    ${SOURCE_FILES})

target_link_libraries(v_import_export Qt5::Core)

