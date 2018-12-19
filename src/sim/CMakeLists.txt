set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{COINDIR})

find_package(Coin REQUIRED)
find_package(SoQt REQUIRED)
find_package(Qt5Core    REQUIRED)
find_package(Qt5Gui    REQUIRED)
find_package(Qt5Widgets     REQUIRED)

if (${Coin_FOUND})
    message(STATUS "Found Coin3D ${Coin_VERSION}")
    message(STATUS "Coin3D Directory: ${Coin_DIR}\n")
endif()
if (${SoQt_FOUND})
    message(STATUS "Found SoQt ${SoQt_VERSION}")
    message(STATUS "SoQt Directory: ${SoQt_DIR}\n")
endif()

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Coin_INCLUDE_DIRS})
include_directories(${SoQt_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CORE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/core")
add_subdirectory(${CORE_DIRECTORY})

set(CMAKE_AUTOMOC ON)

set(HEADER_FILES
    VGeometryProcessor.h
    VGraphicsElement.h
    VGraphicsNode.h
    VGraphicsTriangle.h
    VGraphicsViewer.h
    VLayerAbstractBuilder.h
    VLayerFromAnsysBuilder.h
    VLayerFromFileBuilder.h
    VLayerFromGmeshBuilder.h
    VLayer.h
    VLayerManualBuilder.h
    VLayersProcessor.h
    VModelExport.h
    VModelImport.h
    VModelImportExport.h
    VSimElement.h
    VSimNode.h
    VSimTriangle.h
    VSimulationClass.h
    VSimulationFacade.h
    VSimulator.h
    )

set(SOURCE_FILES
    VGeometryProcessor.cpp
    VGraphicsElement.cpp
    VGraphicsNode.cpp
    VGraphicsTriangle.cpp
    VGraphicsViewer.cpp
    VLayerAbstractBuilder.cpp
    VLayer.cpp
    VLayerFromAnsysBuilder.cpp
    VLayerFromFileBuilder.cpp
    VLayerFromGmeshBuilder.cpp
    VLayerManualBuilder.cpp
    VLayersProcessor.cpp
    VModelExport.cpp
    VModelImport.cpp
    VSimElement.cpp
    VSimNode.cpp
    VSimTriangle.cpp
    VSimulationClass.cpp
    VSimulationFacade.cpp
    VSimulator.cpp
    )

ADD_MODULE(v_sim
    ${HEADER_FILES}
    ${SOURCE_FILES})

add_dependencies(v_sim v_core)
target_link_libraries(v_sim v_core)

target_link_libraries(v_sim Qt5::Core Qt5::Gui Qt5::Widgets)
target_link_libraries(v_sim SoQt::SoQt Coin::Coin)

