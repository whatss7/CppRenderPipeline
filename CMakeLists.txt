cmake_minimum_required(VERSION 3.5)

project(CppRenderPipeline)

# LLVM Part

set(CMAKE_CXX_STANDARD 17)
if(LLVM_DIR)
    find_package(LLVM REQUIRED CONFIG)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

    include_directories(${LLVM_INCLUDE_DIRS})
    separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
    add_definitions(${LLVM_DEFINITIONS_LIST})

    llvm_map_components_to_libnames(llvm_libs core orcjit support nativecodegen)
else()
    message(FATAL_ERROR "LLVM_DIR not set!")
endif()

# Main part
add_compile_options(-fno-rtti)
add_executable(CppRenderPipeline
    src/jit.cpp
    src/main.cpp
)
target_include_directories(CppRenderPipeline
    PUBLIC include
)
target_link_libraries(CppRenderPipeline
    PUBLIC ${llvm_libs}
)
