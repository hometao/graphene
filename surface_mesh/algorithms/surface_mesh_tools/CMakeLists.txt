include_directories(${CMAKE_SOURCE_DIR}/src/)

file(GLOB_RECURSE SRCS ./*.cpp)
file(GLOB_RECURSE HDRS ./*.h)

if(UNIX)
  add_library(graphene_surface_mesh_tools SHARED ${SRCS} ${HDRS})
elseif(WIN32)
  add_library(graphene_surface_mesh_tools STATIC ${SRCS})
endif()

target_link_libraries(graphene_surface_mesh_tools graphene_geometry graphene_surface_mesh)

