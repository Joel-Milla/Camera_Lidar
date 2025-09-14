# cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(CXX_FLAGS "-Wall" "-pedantic")
# set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

# project(camera_fusion)

# find_package(OpenCV REQUIRED)

# include_directories(${OpenCV_INCLUDE_DIRS})
# link_directories(${OpenCV_LIBRARY_DIRS})
# add_definitions(${OpenCV_DEFINITIONS})

# # Executables for exercises
# add_executable (detect_objects src/detect_objects_2.cpp)
# target_link_libraries (detect_objects ${OpenCV_LIBRARIES})

cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(camera_fusion)

find_package(OpenCV REQUIRED)


# Executables
add_executable (detect_objects src/detect_objects_2.cpp)
target_link_libraries (detect_objects ${OpenCV_LIBS})