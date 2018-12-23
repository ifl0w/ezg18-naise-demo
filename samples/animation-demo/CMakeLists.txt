project(animation-demo)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${NAISE_BUILD_ARTIFACTS_DIRECTORY}/${PROJECT_NAME})

include_directories(include)

include_directories(${NAISE_Engine_SOURCE_DIR}/engine/include)

add_executable(${PROJECT_NAME}
        src/main.cpp
        ../common/FPSCameraSystem/FPSCameraInputMapper.cpp
        ../common/FPSCameraSystem/FPSCameraInputMapper.hpp
        ../common/FPSCameraSystem/FPSCameraMovementSystem.cpp
        ../common/FPSCameraSystem/FPSCameraMovementSystem.hpp)

add_dependencies(${PROJECT_NAME} engine)

target_link_libraries(${PROJECT_NAME} engine)

######## COPY ENGINE SHADERS ########
NAISE_ENGINE_COPY_SHARED_LIBS(${PROJECT_NAME})
NAISE_COPY_RESOURCE_DIR()

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

######## COPY COMMON ASSETS ########
# Collect list of common asset files.
set(COMMON_RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../common)
file(GLOB_RECURSE asset_files
        RELATIVE ${COMMON_RESOURCE_DIR}
        ${COMMON_RESOURCE_DIR}/resources/**/*.*
        ${COMMON_RESOURCE_DIR}/resources/*.*)

# This will contain full paths to files in binary directory.
set(binary_files_list)
foreach (file ${asset_files})
    set(source_file ${COMMON_RESOURCE_DIR}/${file})
    set(binary_file ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${file})
    configure_file(${source_file} ${binary_file} COPYONLY)
endforeach ()