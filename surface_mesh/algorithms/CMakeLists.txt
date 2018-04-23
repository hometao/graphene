include_directories(${CMAKE_SOURCE_DIR}/src/)


file(GLOB SRCS
    "decimation/*.cpp"
    "feature_detection/*.cpp"
    "hole_filling/*.cpp"
    "implicit/*.cpp"
    "poisson_reconstruction/*.cpp"
    "mls_reconstruction/*.cpp"
    "rbf_reconstruction/*.cpp"
    "remeshing/*.cpp"
    "reordering/*.cpp"
    "smoothing/*.cpp"
    "subdivision/*.cpp"
    "surface_mesh_tools/*.cpp"
)

file(GLOB HDRS
    "decimation/*.h"
    "feature_detection/*.h"
    "hole_filling/*.h"
    "implicit/*.h"
    "poisson_reconstruction/*.h"
    "mls_reconstruction/*.h"
    "rbf_reconstruction/*.h"
    "remeshing/*.h"
    "reordering/*.h"
    "smoothing/*.h"
    "subdivision/*.h"
    "surface_mesh_tools/*.h"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")


if(UNIX)
  add_library(graphene_surface_mesh_algorithms SHARED ${SRCS} ${HDRS})
elseif(WIN32)
  add_library(graphene_surface_mesh_algorithms STATIC ${SRCS})
endif()

target_link_libraries(graphene_surface_mesh_algorithms
  graphene_surface_mesh
  graphene_geometry
)
