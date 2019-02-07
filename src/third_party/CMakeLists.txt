# Project VARI
# @author Alexey Barashkov

find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport  REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5PrintSupport_INCLUDE_DIRS})

set(CMAKE_AUTOMOC ON)

set(HEADER_FILES
    qcustomplot.h
    )

set(SOURCE_FILES
    qcustomplot.cpp
    )

ADD_MODULE(v_third_party
    ${HEADER_FILES}
    ${SOURCE_FILES})

target_link_libraries(v_third_party Qt5::Core)
target_link_libraries(v_third_party Qt5::Gui)
target_link_libraries(v_third_party Qt5::Widgets)
target_link_libraries(v_third_party Qt5::PrintSupport)

