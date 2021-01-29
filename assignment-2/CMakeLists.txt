cmake_minimum_required(VERSION 3.0.0)
project(assignment-2 VERSION 0.1.0)

include(CTest)
enable_testing()

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(histogram_equalization histogram_equalization.cpp)
add_executable(histogram_matching histogram_matching.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
