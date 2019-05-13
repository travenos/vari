# Project VARI
# @author Alexey Barashkov

set(CMAKE_PREFIX_PATH $ENV{GMSH_DIR} ${CMAKE_PREFIX_PATH})

find_package(GMSH 3.0.6 REQUIRED)
find_package(Qt5Core    REQUIRED)
find_package(Qt5Gui    REQUIRED)

if (${GMSH_FOUND})
    message(STATUS "\nFound GMSH ${GMSH_VERSION}")
    message(STATUS "GMSH Libraries: ${GMSH_LIBRARIES}")
    message(STATUS "GMSH Include directories: ${GMSH_INCLUDE_DIRS}\n")
endif()

include_directories(${GMSH_INCLUDE_DIR})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(HEADER_FILES
    VLayerAbstractBuilder.h
    VLayerFromAnsysBuilder.h
    VLayerFromFileBuilder.h
    VLayerFromGmeshBuilder.h
    VLayerManualBuilder.h
    VLayerFromLayerBuilder.h
    )

set(SOURCE_FILES
    VLayerAbstractBuilder.cpp
    VLayerFromAnsysBuilder.cpp
    VLayerFromFileBuilder.cpp
    VLayerFromGmeshBuilder.cpp
    VLayerManualBuilder.cpp
    VLayerFromLayerBuilder.cpp
    )

ADD_MODULE(v_layer_builders
    ${HEADER_FILES}
    ${SOURCE_FILES})

target_link_libraries(v_layer_builders Qt5::Core)
target_link_libraries(v_layer_builders Qt5::Gui)
target_link_libraries(v_layer_builders ${GMSH_LIBRARIES})
