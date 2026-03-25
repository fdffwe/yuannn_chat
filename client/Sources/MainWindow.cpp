#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <memory>



MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(std::make_unique<Ui::MainWindow>()){
	ui->setupUi(this);

	// 把需要用到的作为自己的成员（指针） + 有一个函数可以把 QDialog 放到中心窗口中（待对比： 有什么区别）
	// 以前是： 每一个窗口都是独立的，都是通过按钮跳转其他的窗口（这样子，以前的窗口是没死的，而且每次点击，都会再次创建一个新的窗口）
	// 做不到复用   

	// 操作：
	// 在mainwindow.h中添加LoginDialog指针成员，然后在构造函数将LoginDialog设置为中心部件
	_login_dlg = new Login(this);
	_login_dlg->setWindowFlags(Qt::Widget);
	this->setCentralWidget(_login_dlg);


	// switchRegister
	connect(_login_dlg, &Login::switchRegister, this, &MainWindow::SlotSwitchReg);

	// 连接登录界面忘记密码信号
    connect(_login_dlg, &Login::switchReset, this, &MainWindow::SlotSwitchReset);
}


MainWindow::~MainWindow() = default;


// 点击注册按钮的槽函数
void MainWindow::SlotSwitchReg(){
    
	_reg_dlg = new RegisterDialog(this);
	_reg_dlg -> setWindowFlags(Qt::Widget);
	
	//连接注册界面返回登录信号
    connect(_reg_dlg, &RegisterDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLogin);
	
	this->setCentralWidget(_reg_dlg);
	_login_dlg->hide();
	_reg_dlg->show(); 
	// 确实是出现了，是不过大小是不符合的（导致看不见，这么多bug）
	// 这个博主也是屌爆了（这就是之前遇见的唯有放： 容器，导致不会随窗口的大小变化而变化）
}


//从注册界面返回登录界面
void MainWindow::SlotSwitchLogin()
{
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    _login_dlg = new Login(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_login_dlg);
    _reg_dlg->hide();
    _login_dlg->show();
    //连接登录界面注册信号
    connect(_login_dlg, &Login::switchRegister, this, &MainWindow::SlotSwitchReg);

    //连接登录界面忘记密码信号
    connect(_login_dlg, &Login::switchReset, this, &MainWindow::SlotSwitchReset);
}

void MainWindow::SlotSwitchReset()
{
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    _reset_dlg = new ResetDialog(this);
    _reset_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_reset_dlg);
    _login_dlg->hide();
    _reset_dlg->show();
    //注册返回登录信号和槽函数
    connect(_reset_dlg, &ResetDialog::switchLogin, this, &MainWindow::SlotSwitchLogin2);
}


//从重置界面返回登录界面
void MainWindow::SlotSwitchLogin2()
{
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    _login_dlg = new Login(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_login_dlg);
    _reset_dlg->hide();
    _login_dlg->show();
    //连接登录界面忘记密码信号
    connect(_login_dlg, &Login::switchReset, this, &MainWindow::SlotSwitchReset);
    //连接登录界面注册信号
    connect(_login_dlg, &Login::switchRegister, this, &MainWindow::SlotSwitchReg);
}