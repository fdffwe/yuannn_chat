cmake_minimum_required(VERSION 3.10)
project(wechat)
set(PROJECT_NAME wechat)


# 生成文件路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)
# 以上都是基本配置

find_package(Boost REQUIRED COMPONENTS 
    system
    filesystem
)

set(JSON_DIR /usr/include/jsoncpp)

set(Sources 
    Sources/main.cpp
    Sources/Server.cpp
    Sources/HttpConnection.cpp
    Sources/LogicSystem.cpp

)
set(Headers 
    Headers/Server.hpp
    Headers/HttpConnection.hpp
    Headers/LogicSystem.hpp
)


add_executable(${PROJECT_NAME}
    ${Sources}
    ${Headers}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Headers
    ${JSON_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Boost::system
    Boost::filesystem
    jsoncpp 
)