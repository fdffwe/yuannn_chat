#include "RegisterDialog.hpp"
#include "ui_RegisterDialog.h"
#include"HttpMgr.hpp"

#include <memory>
#include<QRegularExpression>

// 注意ui里面的名字，应该算是一个对象（而不是一个类，以后规范代码）
RegisterDialog::~RegisterDialog() = default;



RegisterDialog::RegisterDialog(QWidget *parent): QDialog(parent), _ui(std::make_unique<Ui::RegisterDialog>()){
	_ui->setupUi(this);
	// 虽然很多的操作可以在 designer 中设置， 但是在代码中更： 显而易见
	
	// 为密码的输入设置模式
	_ui->pass_edit->setEchoMode(QLineEdit::Password);
	_ui->pass_confirm_edit->setEchoMode(QLineEdit::Password);


	// 收到信号： 通知要注册 
	connect(HttpMgr::getInstance().get(),&HttpMgr::sig_reg_mod_finished,
	this, &RegisterDialog::slot_reg_mod_finished);
	
	// 初始化 http 的回调函数
	initHttpHandlers();
}

void RegisterDialog::slot_reg_mod_finished(ReqId id ,QString res, ErrorCodes err){
	if(err){
		showTip(tr("网络请求错误"),false);
		return ;
	}

	// 解析 Json , res 转为 QByteArray
	QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
	// 失败
	if(jsonDoc.isNull()){
		showTip(tr("json解析失败"),false);
		return;
	}
	// 错误
	if(!jsonDoc.isObject()){
		// 客户端时一样的效果
		// 但是日志处理时： 我们的区分开来， 好debug
		showTip(tr("json解析失败"),false);
		return;
	}

	// 流程： postHttpReq ---> 等待服务器回应 ---> 回调通知模块进行处理
	// 这个函数使用的前提： initHttphandlers , 就是得初始化这个回调函数
	_handlersMap[id](jsonDoc.object());
	return;
}

void RegisterDialog::initHttpHandlers(){

	// 注册： 获取验证码回包的逻辑
	_handlersMap.insert(ReqId::ID_GET_VARIFY_CODE,[this](const QJsonObject& jsonObj){
		int err = jsonObj["error"].toInt();
		if(err){
			showTip(tr("参数错误"),false);
			return;
		}
		auto email = jsonObj["email"].toString();
		showTip("验证码已经发送到邮箱，注意查收",true);
		qDebug() << "email is " << email ; 
	});

	
}




void RegisterDialog::on_get_code_button_clicked(){
	auto email = _ui->email_edit->text();
	// 对于这种多规则的东西的判读： 正则表达式
	// QRegularExpression regex("");
	QRegularExpression regex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
	bool match = regex.match(email).hasMatch();

	if(match){
		// 发送一个 http 的 get_verifycode 请求 --> 获取验证码
		QJsonObject jsonObj;
		jsonObj["email"] = email; 
		HttpMgr::getInstance()->postHttpReq(QUrl(GATE_URL_PREFIX+"/get_verifycode"),jsonObj, ReqId::ID_GET_VARIFY_CODE,Moudles::Register);


		// 错误处理在这里，就是接收 response 的时候处理 （这就是不同模块）
		//slot_reg_mod_finished

	}
	else{
		//提示邮箱不正确
		showTip(tr("邮箱地址不正确"),false);
	}

}

void RegisterDialog::showTip(QString str,bool isOk){
	if(isOk){
		_ui->err_label->setProperty("state","normal");
	}
	else{
		_ui->err_label->setProperty("state","err");
	}
	_ui->err_label->setText(str);
	repolish(_ui->err_label);
}

