set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{COINDIR})

find_package(Coin REQUIRED)
find_package(SoQt REQUIRED)
find_package(Qt5Core    REQUIRED)
find_package(Qt5Gui    REQUIRED)
find_package(Qt5Widgets     REQUIRED)

if (${Coin_FOUND})
    message(STATUS "Found Coin3D ${Coin_VERSION}")
    message(STATUS "Coin3D Directory: ${Coin_DIR}\n")
    link_directories(${Coin_LIB_DIR})
endif()
if (${SoQt_FOUND})
    message(STATUS "Found SoQt ${SoQt_VERSION}")
    message(STATUS "SoQt Directory: ${SoQt_DIR}\n")
    link_directories(${SoQt_LIBRARY_DIR})
endif()

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Coin_INCLUDE_DIRS})
include_directories(${SoQt_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(HEADER_FILES
    VMaterials.h
    VExceptions.h
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
    VNotify.h
    VSimElement.h
    VSimNode.h
    VSimTriangle.h
    VSimulationClass.h
    VSimulationFacade.h
    VSimulationInfo.h
    VSimulationParametres.h
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
    VNotify.cpp
    VSimElement.cpp
    VSimNode.cpp
    VSimTriangle.cpp
    VSimulationClass.cpp
    VSimulationFacade.cpp
    VSimulationParametres.cpp
    VSimulator.cpp
    )

ADD_MODULE(v_sim
    ${HEADER_FILES}
    ${SOURCE_FILES})

target_link_libraries(v_sim Qt5::Core Qt5::Gui Qt5::Widgets)
target_link_libraries(v_sim SoQt::SoQt Coin::Coin)
