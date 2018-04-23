
# for including ui_Main_window...
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../qt/)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Widgets)

add_subdirectory(remeshing_plugin)
add_subdirectory(subdivision_plugin)
add_subdirectory(surface_mesh_io_plugin)
