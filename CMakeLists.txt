cmake_minimum_required (VERSION 3.7)

project (NIntegrate)

find_package (pybind11 REQUIRED)

find_package (Eigen3 REQUIRED)
include_directories (${EIGEN3_INCLUDE_DIR})

find_package (OpenGL REQUIRED)
include_directories (${OPENGL_INCLUDE_DIRS})
link_libraries(${OPENGL_LIBRARIES})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wno-deprecated")

add_library(NIntegrate MODULE
    src/interface_py.cc
    src/domain1d.cc
    src/domain2d.cc
)
target_link_libraries(NIntegrate PRIVATE pybind11::module)

set_target_properties(NIntegrate PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                               SUFFIX "${PYTHON_MODULE_EXTENSION}")