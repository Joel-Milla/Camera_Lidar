cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CXX_FLAGS "-Wall" "-pedantic")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

project(camera_fusion)

find_package(OpenCV REQUIRED)

# Executables for exercises
add_executable (show_lidar_top_view src/show_lidar_top_view.cpp src/structIO.cpp)
target_link_libraries (show_lidar_top_view ${OpenCV_LIBS})

add_executable (project_lidar_to_camera src/project_lidar_to_camera.cpp src/structIO.cpp)
target_link_libraries (project_lidar_to_camera ${OpenCV_LIBS})


