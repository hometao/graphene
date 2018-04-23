include_directories(${CMAKE_CURRENT_BINARY_DIR}
		    ${PROJECT_SOURCE_DIR}/src
		    ${PROJECT_SOURCE_DIR}/src/scenegraph)

add_library(graphene_surface_mesh_io_plugin
  MODULE
  Surface_mesh_io_plugin.cpp
  Surface_mesh_io_plugin.h
)

qt5_use_modules(graphene_surface_mesh_io_plugin Core Widgets)

target_link_libraries(graphene_surface_mesh_io_plugin
  graphene_main
  graphene_scene_graph
  graphene_surface_mesh_scene_graph
  ${QT_LIBRARIES}
)
