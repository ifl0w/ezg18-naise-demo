project(sample-performance-test)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${NAISE_BUILD_ARTIFACTS_DIRECTORY}/${PROJECT_NAME})

include_directories(src)
include_directories(${NAISE_Engine_SOURCE_DIR}/engine/include)

add_executable(sample-performance-test
        src/Game.cpp
        src/Game.hpp
        ../common/FPSCameraSystem/FPSCameraInputMapper.cpp
        ../common/FPSCameraSystem/FPSCameraInputMapper.hpp
        ../common/FPSCameraSystem/FPSCameraMovementSystem.cpp
        ../common/FPSCameraSystem/FPSCameraMovementSystem.hpp
        ../common/VisualDebugging/VisualDebuggingInputMapper.cpp
        ../common/VisualDebugging/VisualDebuggingInputMapper.hpp
        )

add_dependencies(sample-performance-test engine)

target_link_libraries(sample-performance-test SDL2main engine)

######## COPY ENGINE SHADERS ########
set(NAISE_ENGINE_PATH "${NAISE_Engine_SOURCE_DIR}")
set(NAISE_RESOURCE_PATH "${NAISE_Engine_SOURCE_DIR}/engine/resources")
NAISE_COPY_RESOURCE_DIR(${NAISE_ENGINE_PATH} ${NAISE_RESOURCE_PATH})

######## COPY ASSETS ########
# Collect list of asset files.
file(GLOB_RECURSE asset_files
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/**/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/*.*)

# This will contain full paths to files in binary directory.
set(binary_files_list)
foreach (file ${asset_files})
    set(source_file ${CMAKE_CURRENT_SOURCE_DIR}/${file})
    set(binary_file ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${file})
    configure_file(${source_file} ${binary_file} COPYONLY)
endforeach ()