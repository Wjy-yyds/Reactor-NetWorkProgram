cmake_minimum_required(VERSION 3.0)

project(MyServer)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")  # 生成调试信息
#set(CMAKE_BUILD_TYPE Debug)  # 确保是 Debug 模式
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -fdump-record-layouts -Xclang -fdump-vtable-layouts")
find_package(PkgConfig REQUIRED)
pkg_check_modules(LOG4CPP REQUIRED log4cpp)
# Include directory for hello.hpp
include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCES
    src/test_InetAddress.cpp
    src/InetAddress.cpp
    src/NonCopyable.cpp
    src/Acceptor.cpp
    src/Socket.cpp
    src/SocketIO.cpp
    src/TcpConnection.cpp
    src/EventLoop.cpp
)
# Add executable target
add_compile_options(-fstandalone-debug)
message(STATUS "optional: -fstandalone-debug")
add_executable(${PROJECT_NAME} ${SOURCES})
# add_executable(${PROJECT_NAME} src/test_InetAddress.cpp src/InetAddress.cpp)
# target_include_directories(HelloCMake PRIVATE ${LOG4CPP_INCLUDE_DIRS})
# target_link_libraries(HelloCMake PRIVATE ${LOG4CPP_LIBRARIES} pthread)
