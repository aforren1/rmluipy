
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

find_package(Freetype REQUIRED)
add_subdirectory(rmlui)
set_property(TARGET RmlCore PROPERTY POSITION_INDEPENDENT_CODE ON)
add_subdirectory(pybind11)
