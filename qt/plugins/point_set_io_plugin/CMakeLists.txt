include_directories(${CMAKE_CURRENT_BINARY_DIR}
		    ${PROJECT_SOURCE_DIR}/src
		    ${PROJECT_SOURCE_DIR}/src/scenegraph)

set(QT_USE_QTPLUGIN TRUE)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_SHARED)

add_library(
  graphene_point_set_io_plugin
  MODULE
  Point_set_io_plugin.cpp
  Point_set_io_plugin.h
)

qt5_use_modules(graphene_point_set_io_plugin Core Widgets)

target_link_libraries(
  graphene_point_set_io_plugin
  graphene_main
  graphene_scene_graph
  graphene_geometry
)