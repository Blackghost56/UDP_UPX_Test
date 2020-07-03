cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(upx-test)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(VERSION_MAJOR 1)
set(VERSION_MINOR 2)
set(VERSION_PATCH 0)

if(NOT OUT_DIR)
    set(OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR})
message("--->OUT_DIR: ${OUT_DIR}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (QT5_TARGET)
    message("Building with Qt5")
    find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Network
        LinguistTools
        REQUIRED)

    set (QT_LIBRARIES 
        ${Qt5Core_LIBRARIES}
        ${Qt5Gui_LIBRARIES}
        ${Qt5Widgets_LIBRARIES}
        ${Qt5Network_LIBRARIES}
    )
else()
    message("Building with Qt4")
    find_package(Qt4 COMPONENTS
        QtCore
        QtGui
        QtNetwork
        REQUIRED
    )
endif()

set(VERSION_H_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
configure_file(${VERSION_H_DIR}/version.h.in ${VERSION_H_DIR}/version.h @ONLY)


file(GLOB SOURCES
    src/*.cpp
)

file(GLOB HEADERS
    src/*.h    
)


file(GLOB FORMS
    ui/*.ui
)

set(RESOURCES
    res/resources.qrc
)

set(TS_FILES
     translations/${PROJECT_NAME}.ru.ts
)

set (CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

if(QT5_TARGET)
    qt5_wrap_ui(SOURCES ${FORMS})
else()
    qt4_wrap_ui(SOURCES ${FORMS})
endif()

include(QtTranslanion)
# Используем собственные макросы перевода даже в случае Qt5, т.к.
# qt5_create_translation может быть очень медленным для Qt < 5.12.4.
# https://bugreports.qt.io/browse/QTBUG-71354
# Для обновления *.ts файлов использовать команду cmake --build . --target translate или выбрать соответствующую цель вручную в qtcreator
qt_create_translation(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS} ${TS_FILES})
qt_make_translation(QM_FILES ${TS_FILES})

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${FORMS}
    ${RESOURCES}
    ${QM_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d"
     ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin
     ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin
     ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${VERSION_H_DIR}
)
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})

include(CMakeCPack)
