cmake_minimum_required(VERSION 3.9)

find_package(Clang REQUIRED CONFIG)

set(CMAKE_CXX_STANDARD 14)

add_executable(pg_find_signal_handlers pg_find_signal_handlers.cc)

target_include_directories(pg_find_signal_handlers PRIVATE ${LLVM_INCLUDE_DIRS})

target_link_libraries(pg_find_signal_handlers clangTooling)

add_custom_command(TARGET pg_find_signal_handlers POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                           pg_find_signal_handlers bin/pg_find_signal_handlers
                   COMMAND ${CMAKE_COMMAND} -E create_symlink
                           ${CLANG_INSTALL_PREFIX}/lib lib
                   COMMAND ${CMAKE_COMMAND} -E create_symlink
                           ${CLANG_INSTALL_PREFIX}/include include
        BYPRODUCTS
                           bin/pg_find_signal_handlers lib include
                   COMMENT "work around a Clang idiosyncracy")
