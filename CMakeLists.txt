cmake_minimum_required(VERSION 3.27)
project(tudat-examples)
find_package(tudat REQUIRED)
find_package(Eigen3 REQUIRED)
# find_package(nrlmsise00 REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(EIGEN_NO_DEBUG)

add_subdirectory(time_conversion)
add_subdirectory(frame_conversion)
add_subdirectory(propagation)
