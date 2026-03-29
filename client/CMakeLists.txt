cmake_minimum_required(VERSION 3.10)
project(wechat)
set(PROJECT_NAME wechat)

# 使用 qt5_wrap_ui 显式处理 .ui 文件（禁用 AUTOUIC）
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

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
    Sources/TimerBtn.cpp
    Sources/ClickedLabel.cpp
    Sources/clickedbtn.cpp
    Sources/resetdialog.cpp
    Sources/TcpMgr.cpp
    Sources/UserMgr.cpp
    Sources/chatdialog.cpp
    Sources/customizeedit.cpp
    Sources/chatuserlist.cpp
    Sources/chatuserwid.cpp
    Sources/listitembase.cpp
)

set(Headers 
    Headers/Login.hpp
    Headers/MainWindow.hpp
    Headers/RegisterDialog.hpp
    Headers/Global.hpp
    Headers/Singleton.hpp
    Headers/HttpMgr.hpp
    Headers/TimerBtn.hpp
    Headers/ClickedLabel.hpp
    Headers/clickedbtn.hpp
    Headers/resetdialog.hpp
    Headers/TcpMgr.hpp
    Headers/UserMgr.hpp
    Headers/chatdialog.hpp
    Headers/customizeedit.hpp
    Headers/chatuserlist.hpp
    Headers/chatuserwid.hpp
    Headers/listitembase.hpp
)

set(Resources 
    Sources/res.qrc
)

set(UI_FILES
    Forms/ChatDialog.ui
    Forms/Login.ui
    Forms/MainWindow.ui
    Forms/RegisterDialog.ui
    Forms/resetdialog.ui
    Forms/chatuserwid.ui
)

qt5_wrap_ui(UI_SOURCES ${UI_FILES})

add_executable(${PROJECT_NAME}
    ${Sources}
    ${Headers}
    ${Resources}
    ${UI_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Headers
    ${CMAKE_BINARY_DIR}
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
