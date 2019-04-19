# Project VARI
# @author Alexey Barashkov

find_package(Qt5Core    REQUIRED)
find_package(Qt5Gui    REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(HEADER_FILES
    VSimElement.h
    VSimNode.h
    VSimTriangle.h
    VLayer.h
    VNodesVolume.h
    VLayerPolygon.h
    )

set(SOURCE_FILES
    VSimElement.cpp
    VSimNode.cpp
    VSimTriangle.cpp
    VLayer.cpp
    VNodesVolume.cpp
    VLayerPolygon.cpp
    )

ADD_MODULE(v_sim_elements
    ${HEADER_FILES}
    ${SOURCE_FILES})

target_link_libraries(v_sim_elements Qt5::Core Qt5::Gui)
target_link_libraries(v_sim_elements ${OpenCV_LIBS})

