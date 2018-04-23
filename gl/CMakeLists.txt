include_directories(${CMAKE_SOURCE_DIR}/src/)

file(GLOB_RECURSE SRCS ./*.cpp)
file(GLOB_RECURSE HDRS ./*.h)


find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})
find_package(GLUT REQUIRED)

if(UNIX)
  add_library(graphene_gl SHARED ${SRCS} ${HDRS})
elseif(WIN32)
  add_library(graphene_gl STATIC ${SRCS} ${HDRS})
endif()

target_link_libraries(graphene_gl ${GLEW_LIBRARIES} ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})

file(COPY shaders DESTINATION ${PROJECT_BINARY_DIR})