include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt5_wrap_ui(UIS Remeshing_plugin.ui)
add_file_dependencies(${UIS} Remeshing_plugin.ui)

add_library(
  graphene_remeshing_plugin
  MODULE
  Remeshing_plugin.cpp
  Remeshing_plugin.h
  ${UIS}
)

qt5_use_modules(graphene_remeshing_plugin Core Widgets)

target_link_libraries(
  graphene_remeshing_plugin
  graphene_main
  graphene_scene_graph
  graphene_surface_mesh_algorithms
  graphene_surface_mesh_scene_graph
  graphene_utility
)
