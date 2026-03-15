cmake_minimum_required(VERSION 3.10)
project(wechat)
set(PROJECT_NAME wechat)

# 自动处理 ui 等资源文件
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/Forms)

# 生成文件路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)
# 以上都是基本配置


# 需要用到 boost ， beast， Qt 库
find_package(Qt5 REQUIRED COMPONENTS 
    Widgets
    Gui
    Sql
    Core
    Network
)
find_package(Boost REQUIRED COMPONENTS 
    system
    filesystem
)

set(Sources 
    Sources/Login.cpp
    Sources/MainWindow.cpp
    Sources/RegisterDialog.cpp
    Sources/Global.cpp
    Sources/main.cpp
    Sources/HttpMgr.cpp
)
set(Headers 
    Headers/Login.hpp
    Headers/MainWindow.hpp
    Headers/RegisterDialog.hpp
    Headers/Global.hpp
    Headers/Singleton.hpp
    Headers/HttpMgr.hpp
)
set(Resources 
    Resource/res.qrc
)
add_executable(${PROJECT_NAME}
    ${Sources}
    ${Headers}
    ${Resources}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Headers
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Widgets
    Qt5::Gui
    Qt5::Sql
    Qt5::Core
    Qt5::Network
    Boost::system
    Boost::filesystem
)