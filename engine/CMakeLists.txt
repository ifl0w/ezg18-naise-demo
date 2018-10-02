project(engine)

include_directories(include)

file(GLOB_RECURSE SRC_FILES
        src/Engine.cpp
        src/Window.cpp
        src/components/*.cpp
        src/filter/*.cpp
        src/meshes/*.cpp
        src/meshes/*.cpp
        src/scene/*.cpp
        src/systems/RenderSystem.cpp
        src/systems/System.cpp
        src/systems/WindowSystem.cpp
        src/systems/render-engine/Resources.cpp
        src/systems/render-engine/DeferredRenderTarget.cpp
        src/systems/render-engine/RenderEngine.cpp
        #        src/systems/render-engine/PingPongTarget.cpp
        #        src/systems/render-engine/PostProcessingTarget.cpp
        src/systems/render-engine/RenderTarget.cpp
        src/systems/render-engine/lights/LightComponent.cpp
        src/systems/render-engine/materials/*.cpp
        src/systems/render-engine/shaders/*.cpp
        src/systems/render-engine/meshes/*.cpp
        include/Engine.hpp
        include/Window.hpp
        include/components/*.hpp
        include/filter/*.hpp
        include/meshes/*.hpp
        include/meshes/*.hpp
        include/scene/*.hpp
        include/systems/RenderSystem.hpp
        include/systems/System.hpp
        include/systems/WindowSystem.hpp
        include/systems/render-engine/Resources.hpp
        include/systems/render-engine/DeferredRenderTarget.hpp
        include/systems/render-engine/RenderEngine.hpp
        #        include/systems/render-engine/PingPongTarget.hpp
        #        include/systems/render-engine/PostProcessingTarget.hpp
        include/systems/render-engine/RenderTarget.hpp
        include/systems/render-engine/lights/LightComponent.hpp
        include/systems/render-engine/materials/*.hpp
        include/systems/render-engine/shaders/*.hpp
        include/systems/render-engine/meshes/*.hpp
        )

set("OpenGL_GL_PREFERENCE" "GLVND")

### SPDLOG ###
include_directories(external/spdlog/include)

### SDL2 ###
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

### glbinding ###
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glbinding)
include_directories(external/glbinding/source)

### GLM ###
include_directories(external/glm)

### BULLET ###
set(BUILD_PYBULLET OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(USE_DOUBLE_PRECISION ON CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(INSTALL_LIBS OFF CACHE BOOL "" FORCE)
set(BULLET2_USE_THREAD_LOCKS ON CACHE BOOL "" FORCE)

if (CMAKE_FIND_ROOT_PATH)
    set(WTF_BULLET ${CMAKE_FIND_ROOT_PATH})
    unset(CMAKE_FIND_ROOT_PATH)
endif (CMAKE_FIND_ROOT_PATH)

add_subdirectory(external/bullet3)
include_directories(external/bullet3/src)

if (WTF_BULLET)
    set(CMAKE_FIND_ROOT_PATH ${WTF_BULLET})
endif (WTF_BULLET)

add_custom_target(
        bullet
        COMMAND make
        WORKING_DIRECTORY external/bullet3
)

#Set_property(TARGET engine APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${engine_SOURCE_DIR}/external/actor-engine/libcaf_core)
#link_directories(${engine_SOURCE_DIR}/external/actor-engine/libcaf_core)
add_library(engine STATIC ${SRC_FILES} src/systems/input/InputSystem.cpp include/systems/input/InputSystem.hpp src/components/InputComponent.cpp include/components/InputComponent.hpp include/systems/input/InputMapper.hpp)

target_include_directories(engine PUBLIC
        external/spdlog/include
        external/glm
        external/bullet3/src)
target_link_libraries(engine PUBLIC
        SDL2
        glbinding
        glbinding-aux # for additionaly, auxiliary features as logging, meta information, or debugging functionality
        Bullet3Common BulletDynamics BulletCollision LinearMath
        -lpthread)

######## COPY ASSETS ########
# Collect list of asset files.
file(GLOB_RECURSE shader_files
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/assets/
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/**/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/*.*)

# This will contain full paths to files in binary directory.
set(binary_files_list)
foreach (file ${shader_files})
    set(source_file ${CMAKE_CURRENT_SOURCE_DIR}/assets/${file})
    set(binary_file ${CMAKE_CURRENT_BINARY_DIR}/${file})
    configure_file(${source_file} ${binary_file} COPYONLY)
endforeach ()