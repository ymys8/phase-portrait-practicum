cmake_minimum_required(VERSION 3.24)

project(practicum-phisics VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.3.0/mingw_64/lib/cmake")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport)

include(DeployQt)

set(PROJECT_SOURCES
    # cpp
    src/main.cpp
    src/mainwindow.cpp
    src/linear_oscillator_system.cpp
    3rd_party/qcustomplot.cpp

    # headers
    src/mainwindow.h
    src/taskwindow.h
    src/linear_oscillator_system.h
    3rd_party/qcustomplot.h
)

set(FORMS
    ui/mainwindow.ui
    ui/linear_oscillator_system.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(practicum-phisics
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${FORMS}
    )
else()
    add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${FORMS}
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src,
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
)

deploy_qt(${PROJECT_NAME})

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/exe/$<CONFIG>"
    )
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui"
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC 
        "/Zc:__cplusplus"
        "/permissive-"
        "/utf-8"
    )
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
