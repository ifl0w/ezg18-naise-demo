project(samples)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES
        src/**/*.cpp
        src/*.cpp
        include/**/*.hpp
        include/*.hpp
        )

include_directories(include)

include_directories(${NAISE_Engine_SOURCE_DIR}/engine/include)

add_executable(samples ${SRC_FILES})
#add_dependencies(samples engine)

target_link_libraries(samples engine)

######## COPY ASSETS ########
# Collect list of asset files.
file(GLOB_RECURSE shader_files
        RELATIVE ${CMAKE_BINARY_DIR}
        ${CMAKE_BINARY_DIR}/engine/shaders/**/*.*
        ${CMAKE_BINARY_DIR}/engine/shaders/*.*)

# This will contain full paths to files in binary directory.
set(binary_files_list)
foreach(file ${shader_files})
    set(source_file ${CMAKE_BINARY_DIR}/${file})
    set(binary_file ${CMAKE_CURRENT_BINARY_DIR}/${file})
    configure_file(${source_file} ${binary_file} COPYONLY)
endforeach()