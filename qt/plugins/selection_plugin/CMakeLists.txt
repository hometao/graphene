include_directories(
		${CMAKE_CURRENT_BINARY_DIR}
		${PROJECT_SOURCE_DIR}/src
		${PROJECT_SOURCE_DIR}/src/scenegraph
)

add_library(
  graphene_selection_plugin
  MODULE
  Selection_plugin.cpp
  Selection_plugin.h
  Selection_plugin_widget.cpp
  Selection_plugin_widget.h
)

qt5_use_modules(graphene_selection_plugin Core Widgets)

target_link_libraries(
  graphene_selection_plugin
  graphene_main
  graphene_scene_graph
)
