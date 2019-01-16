project(engine)

### FUNCTIONS ###
function(COPY_INCLUDE_DIR source destination)
    file(GLOB_RECURSE include_files
            RELATIVE ${source}
            ${source}/**/*.*
            ${source}/*.*)

    # This will contain full paths to files in binary directory.
    set(binary_files_list)
    foreach (file ${include_files})
        set(source_file ${source}/${file})
        set(binary_file ${destination}/${file})
        configure_file(${source_file} ${binary_file} COPYONLY)
    endforeach ()
endfunction()

### Debug Flags ###
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif ()

if(MSVC)
	add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE)
endif()

add_definitions(-DSPDLOG_TRACE_ON)

include_directories(include)

file(GLOB_RECURSE SRC_FILES
        src/Engine.cpp
        src/Window.cpp
        src/components/*.cpp
        src/filter/*.cpp
        src/factories/*.cpp
        src/factories/*.cpp
        src/scene/*.cpp
        src/systems/RenderSystem.cpp
        src/systems/System.cpp
        src/systems/WindowSystem.cpp
        src/systems/physics/*.cpp
        src/Resources.cpp
        src/systems/render-engine/DeferredRenderTarget.cpp
        src/systems/render-engine/RenderEngine.cpp
        src/systems/render-engine/PingPongTarget.cpp
        src/systems/render-engine/PostProcessingTarget.cpp
        src/systems/render-engine/RenderTarget.cpp
        src/systems/render-engine/lights/*.cpp
        src/systems/render-engine/materials/*.cpp
        src/systems/render-engine/shaders/*.cpp
        src/systems/render-engine/meshes/*.cpp
        src/systems/render-engine/frustum-culling/*.cpp
        src/systems/render-engine/shadow-map/*.cpp
        src/systems/render-engine/textures/*.cpp
        src/systems/particle-system/*.cpp
        src/systems/render-engine/text/*.cpp
        include/Engine.hpp
        include/Window.hpp
        include/components/*.hpp
        include/filter/*.hpp
        include/factories/*.hpp
        include/factories/*.hpp
        include/scene/*.hpp
        include/systems/RenderSystem.hpp
        include/systems/System.hpp
        include/systems/WindowSystem.hpp
        include/Resources.hpp
        include/systems/render-engine/DeferredRenderTarget.hpp
        include/systems/render-engine/RenderEngine.hpp
        include/systems/render-engine/PingPongTarget.hpp
        include/systems/render-engine/PostProcessingTarget.hpp
        include/systems/render-engine/RenderTarget.hpp
        include/systems/render-engine/lights/*.hpp
        include/systems/render-engine/materials/*.hpp
        include/systems/render-engine/shaders/*.hpp
        include/systems/render-engine/meshes/*.hpp
        include/systems/render-engine/frustum-culling/*.hpp
        include/systems/render-engine/shadow-map/*.hpp
        include/systems/render-engine/textures/*.hpp
        include/systems/physics/*.hpp
        include/systems/particle-system/*.hpp
        include/systems/render-engine/text/*.cpp
        )

set(BUILD_SHARED_LIBS OFF)
set("OpenGL_GL_PREFERENCE" "GLVND")

### SPDLOG ###
include_directories(external/spdlog/include)

### SDL2 ###
find_package(SDL2 QUIET)

if (SDL2_FOUND AND NOT MINGW)
    message(">>> SDL2 found!")
else()
    message(">>> Building SDL from source!")
    add_subdirectory(external/sdl2 EXCLUDE_FROM_ALL)

    COPY_INCLUDE_DIR(${SDL2_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include/sdl2/SDL2)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/include/sdl2/)
endif ()

#################
### glbinding ###
#################

find_package(glbinding 3 QUIET)

if (glbinding_FOUND AND NOT MINGW)
    message(">>> glbinding found!")
else()
    message(">>> glbinding not found! It will be compiled from source.")

    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(OPTION_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
    set(OPTION_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(external/glbinding EXCLUDE_FROM_ALL)
    include_directories(external/glbinding/source)

endif ()

### stb ###
include_directories(external/stb)

### json ###
# Typically you don't care so much for a third party library's tests to be
# run from your own project's code.
set(JSON_BuildTests OFF CACHE INTERNAL "")

# Don't use include(nlohmann_json/CMakeLists.txt) since that carries with it
# inintended consequences that will break the build.  It's generally
# discouraged (although not necessarily well documented as such) to use
# include(...) for pulling in other CMake projects anyways.
add_subdirectory(external/nlohmann_json EXCLUDE_FROM_ALL)

### tinygltf ###
include_directories(external/tinygltf)

### GLM ###
include_directories(external/glm)

### BULLET ###
# Compile bullet from source since double precision is targeted

set(BUILD_PYBULLET OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(INSTALL_LIBS OFF CACHE BOOL "" FORCE)
set(BULLET2_USE_THREAD_LOCKS ON CACHE BOOL "" FORCE)
set(USE_DOUBLE_PRECISION ON CACHE BOOL "" FORCE)
add_definitions(-DBT_USE_DOUBLE_PRECISION)

if (CMAKE_FIND_ROOT_PATH)
    set(WTF_BULLET ${CMAKE_FIND_ROOT_PATH})
    unset(CMAKE_FIND_ROOT_PATH)
endif (CMAKE_FIND_ROOT_PATH)

add_subdirectory(external/bullet3 EXCLUDE_FROM_ALL)
include_directories(external/bullet3/src)

if (WTF_BULLET)
    set(CMAKE_FIND_ROOT_PATH ${WTF_BULLET})
endif (WTF_BULLET)

add_custom_target(
        bullet
        COMMAND make
        WORKING_DIRECTORY external/bullet3
)

### FreeType2 ###
if(MSVC)
    # use windows binaries in MSVC
    include_directories(external/windows-binaries/freetype2/include)
    set(FREETYPE_LIBRARIES ${PROJECT_SOURCE_DIR}/external/windows-binaries/freetype2/win64/freetype.lib)
else(MSVC)
    find_package(Freetype REQUIRED)
    include_directories(${FREETYPE_INCLUDE_DIRS})
endif(MSVC)

if (MINGW)
    set(FREETYPE_LIBRARIES "-lfreetype -lharfbuzz -lbz2 -lgraphite2 -lfreetype")
endif ()

### Dear Imgui ###
include_directories(external/imgui)
file(GLOB_RECURSE IMGUI_FILES
        "external/imgui/imgui.cpp"
        "external/imgui/imgui_widgets.cpp"
        "external/imgui/imgui_draw.cpp"
        "external/imgui/imgui_demo.cpp"
        "external/imgui/*.h"
        "include/systems/debugging/imgui-impl/*.hpp"
        "src/systems/debugging/imgui-impl/*.cpp"
)
message(${IMGUI_FILES})
add_library(engine STATIC ${SRC_FILES} ${IMGUI_FILES}
        src/systems/input/InputSystem.cpp include/systems/input/InputSystem.hpp
        src/components/InputComponent.cpp include/components/InputComponent.hpp
        include/systems/input/InputMapper.hpp include/factories/MaterialFactory.hpp
        src/systems/SystemsManager.cpp include/systems/SystemsManager.hpp
        include/systems/Event.hpp src/systems/PhysicsSystem.cpp
        include/systems/PhysicsSystem.hpp src/components/RigidBodyComponent.cpp
        include/components/RigidBodyComponent.hpp src/factories/RigidBodyFactory.cpp
        include/factories/RigidBodyFactory.hpp include/systems/EventManager.hpp
        include/Logger.hpp src/Logger.cpp
        src/systems/render-engine/textures/SkyboxTexture.cpp include/systems/render-engine/textures/SkyboxTexture.hpp
        src/systems/render-engine/meshes/SkyboxMesh.cpp include/systems/render-engine/meshes/SkyboxMesh.hpp
        include/systems/render-engine/shaders/SkyboxShader.hpp src/Skybox.cpp include/Skybox.hpp
        src/systems/TransformSystem.cpp include/systems/TransformSystem.hpp
        include/components/ParentComponent.hpp include/components/AABBComponent.hpp
        include/components/CollisionComponent.hpp include/components/TagComponent.hpp
        include/resource-loader/ModelLoaderAdapter.hpp include/scene/Signature.hpp
        src/systems/animations/AnimationSystem.cpp include/systems/animations/AnimationSystem.hpp
        src/resource-loader/GLTFLoader.cpp include/resource-loader/GLTFLoader.hpp include/components/AnimationComponent.hpp
        include/components/VisualComponent.hpp
        src/systems/particle-system/GPUMeshParticleSystem.cpp include/systems/particle-system/GPUMeshParticleSystem.hpp
        src/systems/particle-system/GPUParticleData.cpp include/systems/particle-system/GPUParticleData.hpp
        include/systems/render-engine/RenderCommands.hpp
        include/systems/render-engine/shadow-map/Cascade.hpp
        include/systems/render-engine/post-processing/HDRPass.cpp include/systems/render-engine/post-processing/HDRPass.hpp
        src/systems/debugging/DebugInputMapper.cpp include/systems/debugging/DebugInputMapper.hpp
        src/systems/debugging/DebugSystem.cpp include/systems/debugging/DebugSystem.hpp)

target_include_directories(engine PUBLIC
        external/spdlog/include
        external/glm
        external/bullet3/src
        external/tinygltf)

if(MSVC)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include/sdl2/)
endif()


if (MINGW OR MSVC)
    target_link_libraries(engine PUBLIC
            glbinding
            glbinding-aux # for additionaly, auxiliary features as logging, meta information, or debugging 
            SDL2main SDL2-static
            ${FREETYPE_LIBRARIES}
            Bullet3Common BulletDynamics BulletCollision LinearMath
            nlohmann_json::nlohmann_json
            -static # link libstdc++, winpthread, gcc_s_seh in mingw
            )
else()
    target_link_libraries(engine PUBLIC
            glbinding
            glbinding-aux # for additionaly, auxiliary features as logging, meta information, or debugging functionality
            SDL2
            ${FREETYPE_LIBRARIES}
            Bullet3Common BulletDynamics BulletCollision LinearMath
            nlohmann_json::nlohmann_json)
endif ()
