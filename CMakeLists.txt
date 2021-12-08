cmake_minimum_required(VERSION 3.0.0)
project(ShmImageProblem VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(
        -Wcast-align
        -Wpedantic
        -Wcast-qual
        -Wconversion
        -Wctor-dtor-privacy
        -Wnon-virtual-dtor
        -Wmissing-include-dirs
        -Wdisabled-optimization
        -Winit-self
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wparentheses
        -Wredundant-decls
        -Wshadow
        -Wsign-promo
        -Wstrict-aliasing
        -Wall
        -Wextra
        -Werror
)

add_subdirectory(external)

get_target_property(boost_include_dirs boost INTERFACE_INCLUDE_DIRECTORIES)

add_executable(image_processor image_processor/main.cpp)
target_include_directories(image_processor SYSTEM PUBLIC ${boost_include_dirs})
target_link_libraries(image_processor PUBLIC rt pthread)

add_executable(image_reader image_reader/main.cpp)
target_include_directories(image_reader SYSTEM PUBLIC ${boost_include_dirs})
target_link_libraries(image_reader PUBLIC rt pthread)

