cmake_minimum_required (VERSION 2.8)
project (libmatrix)

set (LIBNAME matrix)

set (SRC "matrix.c")

add_definitions (-std=c11 -Wall)

add_library (${LIBNAME} SHARED ${SRC})
add_executable (test_matrix main.c)
target_link_libraries (test_matrix ${LIBNAME} m)

install (TARGETS ${LIBNAME} LIBRARY DESTINATION lib)
install (FILES matrix.h DESTINATION include)
