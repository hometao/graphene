include_directories(${PROJECT_SOURCE_DIR}/src)

set(SRCS
    Scene_graph_plugin.cpp
    Scene_graph_plugin.h
    Scene_graph_plugin_widget.cpp
    Scene_graph_plugin_widget.h
    Node_widget.cpp
    Node_widget.h
    Surface_mesh_node_widget.cpp
    Surface_mesh_node_widget.h
    Point_set_node_widget.cpp
    Point_set_node_widget.h
)


if(WITH_KNOTS)
  set(SRCS ${SRCS} Knot_node_widget.cpp Knot_node_widget.h)
endif()

if(WITH_MOLECULES)
  set(SRCS ${SRCS} Molecule_node_widget.cpp Molecule_node_widget.h)
endif()

add_library(graphene_scene_graph_plugin MODULE ${SRCS})


qt5_use_modules(graphene_scene_graph_plugin Core Widgets)

target_link_libraries(
    graphene_scene_graph_plugin
    graphene_scene_graph
    graphene_surface_mesh_scene_graph
    graphene_main
)

if(WITH_KNOTS)
  target_link_libraries(graphene_scene_graph_plugin graphene_knot_scene_graph)
endif()

if(WITH_MOLECULES)
  target_link_libraries(graphene_scene_graph_plugin graphene_molecule_scene_graph)
endif()


