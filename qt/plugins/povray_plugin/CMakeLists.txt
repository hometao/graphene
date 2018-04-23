include_directories(${PROJECT_SOURCE_DIR}/src)

add_library(
    graphene_povray_plugin
    MODULE
    Povray_plugin.cpp
    Povray_plugin.h
    Povray_plugin_widget.cpp
    Povray_plugin_widget.h
    Scene_graph_converter.cpp
    Scene_graph_converter.h
    Node_to_Povray.cpp
    Helper_functions.h
    )

qt5_use_modules(graphene_povray_plugin Core Widgets)

target_link_libraries(
    graphene_povray_plugin
    graphene_scene_graph
    graphene_surface_mesh_scene_graph
    graphene_main
    )

