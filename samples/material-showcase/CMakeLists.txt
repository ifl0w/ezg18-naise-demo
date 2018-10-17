project(material-showcase)
set(CMAKE_CXX_STANDARD 17)

include_directories(include)

include_directories(${NAISE_Engine_SOURCE_DIR}/engine/include)

add_executable(material-showcase
        src/Game.cpp
        src/Game.hpp
        ../common/FPSCameraSystem/FPSCameraInputMapper.cpp
        ../common/FPSCameraSystem/FPSCameraInputMapper.hpp
        ../common/FPSCameraSystem/FPSCameraMovementSystem.cpp
        ../common/FPSCameraSystem/FPSCameraMovementSystem.hpp
        ../common/VisualDebugging/VisualDebuggingInputMapper.cpp ../common/VisualDebugging/VisualDebuggingInputMapper.hpp)

add_dependencies(material-showcase engine)

target_link_libraries(material-showcase engine)

######## COPY ENGINE SHADERS ########
# Collect list of shader files.
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

######## COPY ASSETS ########
# Collect list of asset files.
file(GLOB_RECURSE asset_files
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/**/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/*.*)

# This will contain full paths to files in binary directory.
set(binary_files_list)
foreach (file ${asset_files})
    set(source_file ${CMAKE_CURRENT_SOURCE_DIR}/${file})
    set(binary_file ${CMAKE_CURRENT_BINARY_DIR}/${file})
    configure_file(${source_file} ${binary_file} COPYONLY)
endforeach ()