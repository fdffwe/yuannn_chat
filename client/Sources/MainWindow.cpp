#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <memory>



MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), _ui(std::make_unique<Ui::MainWindow>()){
	_ui->setupUi(this);

	// 把需要用到的作为自己的成员（指针） + 有一个函数可以把 QDialog 放到中心窗口中（待对比： 有什么区别）
	// 以前是： 每一个窗口都是独立的，都是通过按钮跳转其他的窗口（这样子，以前的窗口是没死的，而且每次点击，都会再次创建一个新的窗口）
	// 做不到复用

	// 操作：
	// 在mainwindow.h中添加LoginDialog指针成员，然后在构造函数将LoginDialog设置为中心部件
	login_dialog = new Login(this);
	login_dialog->setWindowFlags(Qt::Widget);
	this->setCentralWidget(login_dialog);


	// switchRegister
	connect(login_dialog, &Login::switchRegister, this, &MainWindow::switchButtonClicked);
}


MainWindow::~MainWindow() = default;


void MainWindow::switchButtonClicked(){
	register_dialog = new RegisterDialog(this);
	register_dialog -> setWindowFlags(Qt::Widget);
	this->setCentralWidget(register_dialog);
	login_dialog->hide();
	register_dialog->show(); 
	// 确实是出现了，是不过大小是不符合的（导致看不见，这么多bug）
	// 这个博主也是屌爆了（这就是之前遇见的唯有放： 容器，导致不会随窗口的大小变化而变化）
}

