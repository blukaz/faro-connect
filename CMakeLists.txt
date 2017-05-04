cmake_minimum_required(VERSION 2.8.3)
project(server_client)

find_package(catkin REQUIRED COMPONENTS
	roscpp
	sensor_msgs
	#actionlib
)

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME ON) 
find_package(Boost 1.60.0 COMPONENTS 
	thread
	asio
	filesystem) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    add_executable(progname file1.cxx file2.cxx) 
    target_link_libraries(progname ${Boost_LIBRARIES})
endif()

catkin_package(
	LIBRARIES ${server_client}
	CATKIN_DEPENDS
)

include_directories(include)
include_directories(
	${catkin_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

add_executable(server_client src/server_client.cpp)
target_link_libraries(server_client ${catkin_LIBRARIES})
