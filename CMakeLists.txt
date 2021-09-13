cmake_minimum_required(VERSION 3.4...3.18)
set(CMAKE_CXX_STANDARD 14)
project(rmluipy)

add_subdirectory(submodules)
pybind11_add_module(rmluipy src/main.cpp)

# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code as a
# define (VERSION_INFO) here.
# target_compile_definitions(rmlui PRIVATE)
set_target_properties(rmluipy PROPERTIES OUTPUT_NAME "rmlui")
target_link_libraries(rmluipy PUBLIC ${FREETYPE_LIBRARIES} RmlCore)
target_include_directories(rmluipy PRIVATE ${FREETYPE_INCLUDE_DIRS} rmlui/Include)
target_compile_definitions(rmluipy PRIVATE RMLUI_STATIC_LIB)
