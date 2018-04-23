
add_library(
  graphene_subdivision_plugin
  MODULE
  Subdivision_plugin.cpp
  Subdivision_plugin.h
)

qt5_use_modules(graphene_subdivision_plugin Core Widgets)

target_link_libraries(
  graphene_subdivision_plugin
  graphene_main
  graphene_scene_graph
  graphene_surface_mesh_algorithms
  graphene_surface_mesh_scene_graph
)
