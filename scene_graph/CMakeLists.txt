find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

set(SCENE_SRCS
  Scene_graph.cpp
  Scene_graph.h
  Base_node.cpp
  Base_node.h
  Operations.h
  Object_node.cpp
  Object_node.h
  Lasso_selection_node.cpp
  Lasso_selection_node.h
)


if(UNIX)
  add_library(graphene_scene_graph SHARED ${SCENE_SRCS})
elseif(WIN32)
  add_library(graphene_scene_graph STATIC ${SCENE_SRCS})
endif()

target_link_libraries(graphene_scene_graph
  graphene_geometry
  graphene_gl
  ${GLEW_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${GLUT_LIBRARIES}
)
