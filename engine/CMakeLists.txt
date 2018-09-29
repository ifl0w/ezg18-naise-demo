project(engine)

include_directories(include)

file(GLOB_RECURSE SRC_FILES
        src/**/*.cpp
        src/*.cpp
        include/**/*.hpp
        include/*.hpp
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
add_library(engine STATIC ${SRC_FILES})

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
