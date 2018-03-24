# vim: tabstop=4 shiftwidth=4 softtabstop=4 expandtab
cmake_minimum_required(VERSION 3.4)

if (YO_LLVM_PATH)
    find_package(Clang REQUIRED
        HINTS ${YO_LLVM_PATH}
        NO_DEFAULT_PATH
        CONFIG
        )
else()
    find_package(Clang REQUIRED
        CONFIG)
endif()

SET(CMAKE_CXX_STANDARD 14)

add_executable(yo
    main.cc)

target_include_directories(yo
    PRIVATE
    ${LLVM_INCLUDE_DIRS}
    )

target_link_libraries(yo
    clangTooling
    clangBasic
    clangASTMatchers
    )

add_custom_command(TARGET yo
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different yo bin/yo
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CLANG_INSTALL_PREFIX}/lib lib
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CLANG_INSTALL_PREFIX}/include include
    BYPRODUCTS bin/yo lib include
    COMMENT "work around a Clang idiosyncracy"
    )
