cmake_minimum_required(VERSION 2.8.3)

project(robo1)

find_package(catkin REQUIRED COMPONENTS roscpp)

catkin_package()

include_directories(include ${catkin_INCLUDE_DIRS})


add_executable(hello hello.cpp)
add_executable(hello1 hello1.cpp)


target_link_libraries(hello ${catkin_LIBRARIES})
target_link_libraries(hello1 ${catkin_LIBRARIES})