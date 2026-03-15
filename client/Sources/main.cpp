#include <QApplication>
#include <QDebug>
#include<QDir>
#include <QIcon>
#include <QFile>
#include<QSettings>


#include "Login.hpp"
#include "MainWindow.hpp"
#include "Global.hpp"   

// 一些按钮的图片资源是： .icon 类型的文件



int main(int argc, char * argv[]){
    Q_INIT_RESOURCE(res);
    QApplication app(argc,argv);

    QFile qss(":/style/styleSheets.qss");
    qss.open(QFile::ReadOnly);
    QString style = qss.readAll();
    app.setStyleSheet(style);

    
    // config.ini 参数设置
    // toNativeSeparators 转化为本地分隔符
    // QDir 处理路径， QSettings
    QString tmpPath = "/home/eai/demo/weChat/client/config.ini";
    QString configPath = QDir::toNativeSeparators(tmpPath); 

    // 文件管理对象
    QSettings settings(configPath, QSettings::IniFormat); 
     
    // 获取文件中的参数
    QString gateHost = settings.value("GateServer/host").toString(); 
    QString gatePort = settings.value("GateServer/port").toString(); 

    GATE_URL_PREFIX = "http://" + gateHost + ":" + gatePort + "";

    qDebug() << GATE_URL_PREFIX ; 
    // 客户端的启动
    MainWindow w;
    w.show();
    return app.exec();
}



