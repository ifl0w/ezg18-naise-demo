project(sample-demo)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${NAISE_BUILD_ARTIFACTS_DIRECTORY}/${PROJECT_NAME})

include_directories(include)
include_directories(${NAISE_Engine_SOURCE_DIR}/engine/include)

add_executable(sample-demo
        src/Demo.cpp
        src/Demo.hpp
        ../common/FPSCameraSystem/FPSCameraInputMapper.cpp
        ../common/FPSCameraSystem/FPSCameraInputMapper.hpp
        ../common/FPSCameraSystem/FPSCameraMovementSystem.cpp
        ../common/FPSCameraSystem/FPSCameraMovementSystem.hpp
        ../common/VisualDebugging/VisualDebuggingInputMapper.cpp
        ../common/VisualDebugging/VisualDebuggingInputMapper.hpp
        src/SceneLoaderAdapter.hpp
        src/SceneLoaderAdapter.cpp
        src/CameraSelectionSystem/CameraSelectionInputMapper.hpp
        src/CameraSelectionSystem/CameraSelectionInputMapper.cpp src/CameraSelectionSystem/CameraSelectionSystem.cpp src/CameraSelectionSystem/CameraSelectionSystem.hpp src/CameraSelectionSystem/DemoEvents.hpp)

add_dependencies(sample-demo engine)

target_link_libraries(sample-demo engine)

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

#file(GLOB_RECURSE RES_FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/**/*)
#add_custom_target(dummy_target SOURCES ${RES_FILES})
#add_dependencies(${PROJECT_NAME} dummy_target)
#
#NAISE_COPY_DIRECTORY_POST_BUILD(dummy_target
#        ${CMAKE_CURRENT_SOURCE_DIR}/resources/
#        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/)