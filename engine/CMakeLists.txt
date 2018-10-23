project(engine)

### Debug Flags ###
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif ()

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
        src/systems/render-engine/lights/PointLight.cpp
        src/systems/render-engine/lights/DirectionalLight.cpp
        src/systems/render-engine/lights/Light.cpp
        src/systems/render-engine/materials/*.cpp
        src/systems/render-engine/shaders/*.cpp
        src/systems/render-engine/meshes/*.cpp
        src/systems/render-engine/frustum-culling/*.cpp
        src/systems/render-engine/shadow-map/*.cpp
        src/systems/render-engine/textures/*.cpp
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
        include/systems/render-engine/lights/DirectionalLight.hpp
        include/systems/render-engine/lights/PointLight.hpp
        include/systems/render-engine/lights/Light.hpp
        include/systems/render-engine/materials/*.hpp
        include/systems/render-engine/shaders/*.hpp
        include/systems/render-engine/meshes/*.hpp
        include/systems/render-engine/frustum-culling/*.hpp
        include/systems/render-engine/shadow-map/*.hpp
        include/systems/render-engine/textures/*.hpp
        include/systems/physics/*.hpp
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
    add_subdirectory(external/sdl2)
    include_directories(external/sdl2/include)
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

    add_subdirectory(external/glbinding)
    include_directories(external/glbinding/source)
endif ()

### stb ###
include_directories(external/stb)

### tinygltf ###
include_directories(external/tinygltf)

### GLM ###
include_directories(external/glm)

### BULLET ###

find_package(Bullet 2.87)

if (Bullet_FOUND AND NOT MINGW)
    message(">>> Bullet found!")
else()
    message(">>> Bullet not found! It will be compiled from source.")

    set(BUILD_PYBULLET OFF CACHE BOOL "" FORCE)
    set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
    set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
    set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
    set(INSTALL_LIBS OFF CACHE BOOL "" FORCE)
    set(BULLET2_USE_THREAD_LOCKS ON CACHE BOOL "" FORCE)
#    set(USE_DOUBLE_PRECISION ON CACHE BOOL "" FORCE)
#    add_definitions(-DBT_USE_DOUBLE_PRECISION)

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

endif ()


add_library(engine STATIC ${SRC_FILES}
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
        include/components/ParentComponent.hpp include/components/AABBComponent.hpp include/components/CollisionComponent.hpp)

target_include_directories(engine PUBLIC
        external/spdlog/include
        external/glm
        external/bullet3/src
        external/tinygltf)

if (MINGW)
    target_link_libraries(engine PUBLIC
            glbinding
            glbinding-aux # for additionaly, auxiliary features as logging, meta information, or debugging functionality
            SDL2-static
            Bullet3Common BulletDynamics BulletCollision LinearMath -static)
else()
    target_link_libraries(engine PUBLIC
            glbinding
            glbinding-aux # for additionaly, auxiliary features as logging, meta information, or debugging functionality
            SDL2
            Bullet3Common BulletDynamics BulletCollision LinearMath)
endif ()

######## COPY ASSETS ########
add_custom_command(TARGET engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders
        $<TARGET_FILE_DIR:engine>/engine/shaders)
