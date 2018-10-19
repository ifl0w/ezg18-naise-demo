cmake_minimum_required(VERSION 3.10)
project(NAISE_Engine)

set(CMAKE_CXX_STANDARD 17)

# Define Release by default.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    message(STATUS "Build type not specified: Use Release by default.")
endif(NOT CMAKE_BUILD_TYPE)

### Debug Flags ###
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif ()

### Clang std lib ###
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

set(NAISE_BUILD_ARTIFACTS_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/artifacts)

subdirs(engine samples)

### functions ###

# This function sets up a post build command that copies the required resources to the
# build folder of the software that is using this engine.
function(NAISE_COPY_RESOURCES_POST_BUILD target)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            $<TARGET_FILE_DIR:engine>/engine
            $<TARGET_FILE_DIR:${target}>/engine)
endfunction(NAISE_COPY_RESOURCES_POST_BUILD PARENT_SCOPE)

function(NAISE_COPY_RESOURCE_DIR NAISE_ENGINE_PATH NAISE_RESOURCE_PATH)
    NAISE_BUILD_LOG("NAISE >> Engine base path: ${NAISE_ENGINE_PATH}")
    NAISE_BUILD_LOG("Resource path: ${NAISE_RESOURCE_PATH}")

    file(GLOB_RECURSE shader_files
            RELATIVE ${NAISE_ENGINE_PATH}
            ${NAISE_RESOURCE_PATH}/**/*.*
            ${NAISE_RESOURCE_PATH}*.*)

    # This will contain full paths to files in binary directory.
    set(binary_files_list)
    foreach(file ${shader_files})
        set(source_file ${NAISE_ENGINE_PATH}/${file})
        set(binary_file ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${file})
        configure_file(${source_file} ${binary_file} COPYONLY)
    endforeach()

    NAISE_BUILD_LOG("Copied engine resources!")

endfunction(NAISE_COPY_RESOURCE_DIR PARENT_SCOPE)

function(NAISE_BUILD_LOG message)
    message("NAISE Engine >> ${message}")
endfunction(NAISE_BUILD_LOG PARENT_SCOPE)