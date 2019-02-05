# Project VARI
# @author Alexey Barashkov

find_package(Qt5Core    REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(HEADER_FILES
    VLayerAbstractBuilder.h
    VLayerFromAnsysBuilder.h
    VLayerFromFileBuilder.h
    VLayerFromGmeshBuilder.h
    VLayerManualBuilder.h
    )

set(SOURCE_FILES
    VLayerAbstractBuilder.cpp
    VLayerFromAnsysBuilder.cpp
    VLayerFromFileBuilder.cpp
    VLayerFromGmeshBuilder.cpp
    VLayerManualBuilder.cpp
    )

ADD_MODULE(v_layer_builders
    ${HEADER_FILES}
    ${SOURCE_FILES})

target_link_libraries(v_layer_builders Qt5::Core)

