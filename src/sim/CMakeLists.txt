SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{COINDIR})

FIND_PACKAGE(Coin REQUIRED)
FIND_PACKAGE(SoQt REQUIRED)
FIND_PACKAGE(Qt5Core    REQUIRED)
FIND_PACKAGE(Qt5Gui    REQUIRED)

IF (${Coin_FOUND})
    MESSAGE(STATUS "Found Coin3D ${Coin_VERSION}")
    MESSAGE(STATUS "Coin3D Directory: ${Coin_DIR}\n")
    link_directories(${Coin_LIB_DIR})
ENDIF()
IF (${SoQt_FOUND})
    MESSAGE(STATUS "Found SoQt ${SoQt_VERSION}")
    MESSAGE(STATUS "SoQt Directory: ${SoQt_DIR}\n")
    link_directories(${SoQt_LIBRARY_DIR})
ENDIF()

INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Gui_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Coin_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${SoQt_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

SET(HEADER_FILES
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
    VCloth.h
    VSimElement.h
    VSimNode.h
    VSimTriangle.h
    VSimulationClass.h
    VSimulationFacade.h
    VSimulationParametres.h
    VSimulator.h
    )

SET(SOURCE_FILES
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

TARGET_LINK_LIBRARIES(v_sim Qt5::Core Qt5::Gui)
TARGET_LINK_LIBRARIES(v_sim SoQt::SoQt Coin::Coin)
