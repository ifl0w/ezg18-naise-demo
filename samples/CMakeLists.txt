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
add_dependencies(samples engine)

target_link_libraries(samples engine)
