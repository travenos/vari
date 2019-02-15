# Project VARI
# @author Alexey Barashkov

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{COINDIR})

find_package(Coin REQUIRED)
find_package(SoQt REQUIRED)
find_package(Qt5Core    REQUIRED)
find_package(Qt5Gui    REQUIRED)
find_package(Qt5Widgets     REQUIRED)

if (${Coin_FOUND})
    message(STATUS "Found Coin3D ${Coin_VERSION}")
    message(STATUS "Coin3D Directory: ${Coin_DIR}")
    message(STATUS "Coin3D Include directories: ${Coin_INCLUDE_DIR}\n")
endif()
if (${SoQt_FOUND})
    message(STATUS "Found SoQt ${SoQt_VERSION}")
    message(STATUS "SoQt Directory: ${SoQt_DIR}")
    message(STATUS "SoQt Include directories: ${SoQt_INCLUDE_DIRS}\n")
endif()

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Coin_INCLUDE_DIR})
include_directories(${SoQt_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(STRUCTURES_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/structures")
add_subdirectory(${STRUCTURES_DIRECTORY})
set(GRAPHICS_ELEMENTS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/graphics_elements")
add_subdirectory(${GRAPHICS_ELEMENTS_DIRECTORY})
set(IMPORT_EXPORT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/import_export")
add_subdirectory(${IMPORT_EXPORT_DIRECTORY})
set(LAYER_BUILDERS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/layer_builders")
add_subdirectory(${LAYER_BUILDERS_DIRECTORY})
set(SIM_ELEMENTS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/sim_elements")
add_subdirectory(${SIM_ELEMENTS_DIRECTORY})

set(CMAKE_AUTOMOC ON)

set(HEADER_FILES
    VGraphicsViewer.h
    VLayersProcessor.h
    VSimulationClass.h
    VSimulationFacade.h
    VSimulator.h
    )

set(SOURCE_FILES
    VGraphicsViewer.cpp
    VLayersProcessor.cpp
    VSimulationClass.cpp
    VSimulationFacade.cpp
    VSimulator.cpp
    )

ADD_MODULE(v_sim
    ${HEADER_FILES}
    ${SOURCE_FILES})

add_dependencies(v_sim
    v_structures
    v_graphics_elements
    v_import_export
    v_layer_builders
    v_sim_elements)
target_link_libraries(v_sim
    v_structures
    v_graphics_elements
    v_import_export
    v_layer_builders
    v_sim_elements)

target_link_libraries(v_sim Qt5::Core Qt5::Gui Qt5::Widgets)
target_link_libraries(v_sim SoQt::SoQt Coin::Coin)
