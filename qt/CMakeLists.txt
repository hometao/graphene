include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(Qt5Widgets)
find_package(Qt5OpenGL)

set(CMAKE_AUTOMOC ON)

set(SRCS
  Main_window.cpp
  Main_window.h
  ui_Main_window.h
  QGL_viewer_widget.cpp
  QGL_viewer_widget.h
  Qdebug_stream.h)

set(QRCS Main_window.qrc)
set(UIS Main_window.ui About_graphene_dialog.ui)

qt5_add_resources(QRC_SRCS ${QRCS})
qt5_wrap_ui(UI_HS ${UIS})


if(WIN32)
    add_library(
        graphene_main
        STATIC
        ${SRCS}
        ${UI_HS}
        ${QRC_SRCS}
)
else()
    add_library(
        graphene_main
        SHARED
        ${SRCS}
        ${UI_HS}
        ${QRC_SRCS}
)
endif()

qt5_use_modules(graphene_main Widgets OpenGL)

target_link_libraries(graphene_main
  graphene_scene_graph
  graphene_gl
  ${QT_LIBRARIES}
  ${GLEW_LIBRARIES}
)

add_executable(graphene
  ${GUI_TYPE}
  graphene.cpp)

qt5_use_modules(graphene Widgets OpenGL)

target_link_libraries(graphene
  graphene_main
)


add_subdirectory(plugins)