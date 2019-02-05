# Project VARI
# @author Alexey Barashkov

find_package(Qt5Core    REQUIRED)
find_package(Qt5Gui    REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(HEADER_FILES
    VExceptions.h
    VMaterials.h
    VNotify.h
    VSimulationInfo.h
    VSimulationParameters.h
    )

set(SOURCE_FILES
    VNotify.cpp
    VSimulationParameters.cpp
    )

ADD_MODULE(v_structures
    ${HEADER_FILES}
    ${SOURCE_FILES})

target_link_libraries(v_structures Qt5::Core Qt5::Gui)

