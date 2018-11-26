INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

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
    VMaterial.h
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

TARGET_LINK_LIBRARIES(v_sim Qt5::Core)
IF(WIN32)
    TARGET_LINK_LIBRARIES(v_sim soqt1)
    TARGET_LINK_LIBRARIES(v_sim coin4)
ELSE()
    TARGET_LINK_LIBRARIES(v_sim SoQt Coin)
ENDIF()
