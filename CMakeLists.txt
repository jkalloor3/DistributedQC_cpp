# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.5)

# Set the project name
project (dist_qc)

# Create a sources variable with a link to all cpp files to compile
set(SOURCES
    src/modules/stdmods.cpp
    src/modules/shormods.cpp
    src/main.cpp
    src/modules/module.cpp
)

# Add an executable with the above sources
add_executable(dist_qc ${SOURCES})

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(dist_qc
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)