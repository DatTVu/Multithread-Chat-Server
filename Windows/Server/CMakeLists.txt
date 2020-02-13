#Set the minimum version of CMkae that can be used
#To find the cmake version run
#$cmake --version`
cmake_minimum_required(VERSION 3.5)

#Set the project name
project (server)

#Set C++ Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#Create a sources variable with a link to all cpp files to compile
set(SOURCES
	src/server.cpp
	src/myqueue.cpp
)
#Add an executable with the above sources
add_executable(${PROJECT_NAME} ${SOURCES})
#Set the directories that should be included in the build comman for this project
target_include_directories(${PROJECT_NAME} PUBLIC
	${PROJECT_SOURCE_DIR}/include
)

