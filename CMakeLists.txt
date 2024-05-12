cmake_minimum_required(VERSION 3.10)
project(phc_read_write_app)

set(CMAKE_CXX_STANDARD 14)

add_executable(phc_read_write_app src/main.cpp)

# Link libcap library
find_library(LIBCAP_LIBRARY cap)
target_link_libraries(phc_read_write_app ${LIBCAP_LIBRARY})
