find_package(Qt5Core    REQUIRED)
find_package(Coin REQUIRED)
find_package(Qt5Gui    REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Coin_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(HEADER_FILES
    VGraphicsElement.h
    VGraphicsLayer.h
    VGraphicsNode.h
    VGraphicsTriangle.h
    )

set(SOURCE_FILES
    VGraphicsElement.cpp
    VGraphicsLayer.cpp
    VGraphicsNode.cpp
    VGraphicsTriangle.cpp
    )

ADD_MODULE(v_graphics_elements
    ${HEADER_FILES}
    ${SOURCE_FILES})

target_link_libraries(v_graphics_elements Qt5::Core Qt5::Gui Coin::Coin)

