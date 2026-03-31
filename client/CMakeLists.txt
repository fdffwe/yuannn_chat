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
    Sources/adduseritem.cpp
    Sources/applyfriend.cpp
    Sources/applyfrienditem.cpp
    Sources/applyfriendlist.cpp
    Sources/applyfriendpage.cpp
    Sources/authenfriend.cpp
    Sources/BubbleFrame.cpp
    Sources/chatdialog.cpp
    Sources/ChatItemBase.cpp
    Sources/chatpage.cpp
    Sources/chatuserlist.cpp
    Sources/chatuserwid.cpp
    Sources/ChatView.cpp
    Sources/clickedbtn.cpp
    Sources/clickedlabel.cpp
    Sources/clickedoncelabel.cpp
    Sources/contactuserlist.cpp
    Sources/conuseritem.cpp
    Sources/customizeedit.cpp
    Sources/customizetextedit.cpp
    Sources/findfaildlg.cpp
    Sources/findsuccessdlg.cpp
    Sources/friendinfopage.cpp
    Sources/friendlabel.cpp
    Sources/global.cpp
    Sources/grouptipitem.cpp
    Sources/httpmgr.cpp
    Sources/imagecropperlabel.cpp
    Sources/invaliditem.cpp
    Sources/lineitem.cpp
    Sources/listitembase.cpp
    Sources/loadingdlg.cpp
    Sources/logindialog.cpp
    Sources/main.cpp
    Sources/mainwindow.cpp
    Sources/MessageTextEdit.cpp
    Sources/PictureBubble.cpp
    Sources/registerdialog.cpp
    Sources/resetdialog.cpp
    Sources/searchlist.cpp
    Sources/statelabel.cpp
    Sources/statewidget.cpp
    Sources/tcpmgr.cpp
    Sources/TextBubble.cpp
    Sources/timerbtn.cpp
    Sources/userdata.cpp
    Sources/userinfopage.cpp
    Sources/usermgr.cpp
)

set(Headers 
    Headers/adduseritem.h
    Headers/applyfriend.h
    Headers/applyfrienditem.h
    Headers/applyfriendlist.h
    Headers/applyfriendpage.h
    Headers/authenfriend.h
    Headers/BubbleFrame.h
    Headers/chatdialog.h
    Headers/ChatItemBase.h
    Headers/chatpage.h
    Headers/chatuserlist.h
    Headers/chatuserwid.h
    Headers/ChatView.h
    Headers/clickedbtn.h
    Headers/clickedlabel.h
    Headers/clickedoncelabel.h
    Headers/contactuserlist.h
    Headers/conuseritem.h
    Headers/customizeedit.h
    Headers/customizetextedit.h
    Headers/findfaildlg.h
    Headers/findsuccessdlg.h
    Headers/friendinfopage.h
    Headers/friendlabel.h
    Headers/global.h
    Headers/grouptipitem.h
    Headers/httpmgr.h
    Headers/imagecropperdialog.h
    Headers/imagecropperlabel.h
    Headers/invaliditem.h
    Headers/lineitem.h
    Headers/listitembase.h
    Headers/loadingdlg.h
    Headers/logindialog.h
    Headers/mainwindow.h
    Headers/MessageTextEdit.h
    Headers/PictureBubble.h
    Headers/registerdialog.h
    Headers/resetdialog.h
    Headers/searchlist.h
    Headers/singleton.h
    Headers/statelabel.h
    Headers/statewidget.h
    Headers/tcpmgr.h
    Headers/TextBubble.h
    Headers/timerbtn.h
    Headers/userdata.h
    Headers/userinfopage.h
    Headers/usermgr.h
)

set(Resources 
    rc.qrc
)

set(UI_FILES
    Forms/adduseritem.ui
    Forms/applyfriend.ui
    Forms/applyfrienditem.ui
    Forms/applyfriendpage.ui
    Forms/authenfriend.ui
    Forms/chatdialog.ui
    Forms/chatpage.ui
    Forms/chatuserwid.ui
    Forms/conuseritem.ui
    Forms/findfaildlg.ui
    Forms/findsuccessdlg.ui
    Forms/friendinfopage.ui
    Forms/friendlabel.ui
    Forms/grouptipitem.ui
    Forms/lineitem.ui
    Forms/loadingdlg.ui
    Forms/logindialog.ui
    Forms/mainwindow.ui
    Forms/registerdialog.ui
    Forms/resetdialog.ui
    Forms/userinfopage.ui
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
