# This toolchain works with the cross compiler made in this tutorial: http://www.kitware.com/blog/home/post/426

# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
SET(CMAKE_C_COMPILER
/home/victor/x-tools/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-gcc)

SET(CMAKE_CXX_COMPILER
/home/victor/x-tools/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-g++)

# where is the target environment
SET(CMAKE_FIND_ROOT_PATH /home/victor/RaspBerryPi/rootfs_raspbian)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

