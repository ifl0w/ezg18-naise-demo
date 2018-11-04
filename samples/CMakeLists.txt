cmake_minimum_required(VERSION 3.10)
project(NAISE_Engine_samples)

set(CMAKE_CXX_STANDARD 17)

if (MINGW)
    # hack needed to compile in mingw
    # Source: http://forums.libsdl.org/viewtopic.php?p=49618
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lmingw32 -lSDL2main")
endif ()

subdirs(demo
        performance-test
        material-showcase
        animation-demo)