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
    VSimulationParametres.h
    )

set(SOURCE_FILES
    VNotify.cpp
    VSimulationParametres.cpp
    )

ADD_MODULE(v_core
    ${HEADER_FILES}
    ${SOURCE_FILES})

target_link_libraries(v_core Qt5::Core Qt5::Gui)

