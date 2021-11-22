set(CMAKE_C_COMPILER "$ENV{HOME}/toolchain/bin/aarch64-linux-android21-clang")
set(CMAKE_CXX_COMPILER "$ENV{HOME}/toolchain/bin/aarch64-linux-android21-clang++")

set(CROSS_PREFIX "aarch64-linux-android")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH} -DANDROID")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH} -DANDROID")
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}")