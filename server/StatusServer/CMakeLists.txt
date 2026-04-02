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

# 查找依赖包 gRPC
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
find_package(Threads REQUIRED)

set(JSON_DIR /usr/include/jsoncpp)
set(HIREDIS_DIR /usr/include/hiredis)
set(CPPCONN_DIR /usr/include/cppconn)


set(Sources 
    Sources/ConfigMgr.cpp
    Sources/message.pb.cc
    Sources/message.grpc.pb.cc
    Sources/RedisMgr.cpp
    Sources/MysqlDAO.cpp
    Sources/MysqlMgr.cpp
    Sources/StatusServiceImpl.cpp
    Sources/StatusServer.cpp
    Sources/DistLock.cpp



)
set(Headers 
    Headers/ConfigMgr.hpp
    Headers/message.pb.h
    Headers/message.grpc.pb.h
    Headers/RedisMgr.hpp
    Headers/MysqlDAO.hpp
    Headers/MysqlMgr.hpp
    Headers/StatusServiceImpl.hpp
    Headers/DistLock.hpp

)



add_executable(${PROJECT_NAME}
    ${Sources}
    ${Headers}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Headers
    ${CMAKE_SOURCE_DIR}/Sources
    ${JSON_DIR}
    ${REDIS_DIR}
    ${CPPCONN_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    # asio 库
    Boost::system
    Boost::filesystem
    jsoncpp 
    # gprc 服务
    gRPC::grpc++
    protobuf::libprotobuf
    Threads::Threads
    # redis 服务
    hiredis
    # mysql 服务
    mysqlcppconn
)