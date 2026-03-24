#include "Login.hpp"
#include "ui_Login.h"
#include <memory>





Login::Login(QWidget *parent): QDialog(parent), _ui(std::make_unique<Ui::Login>()){
	_ui->setupUi(this);

	_ui->passwdEidt->setEchoMode(QLineEdit::Password);



	// 发送我要注册的信号， 由 MainWindow 统一管理
	connect(_ui->registerButton, &QPushButton::clicked, this, &Login::switchRegister);


	_ui->forget_label->SetState("normal","hover","","selected","selected_hover","");
    _ui->forget_label->setCursor(Qt::PointingHandCursor);
    connect(_ui->forget_label, &ClickedLabel::clicked, this, &Login::slot_forget_pwd);
}

Login::~Login() = default;

void Login::slot_forget_pwd()
{
    qDebug()<<"slot forget pwd";
    emit switchReset();
}
