include_directories(${CMAKE_SOURCE_DIR}/src/)

file(GLOB_RECURSE SRCS ./*.cpp)
file(GLOB_RECURSE HDRS ./*.h)

if(UNIX)
  add_library(graphene_geometry SHARED ${SRCS} ${HDRS})
elseif(WIN32)
  add_library(graphene_geometry STATIC ${SRCS})
endif()

