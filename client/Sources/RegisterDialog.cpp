#include "RegisterDialog.hpp"
#include "ui_RegisterDialog.h"
#include"HttpMgr.hpp"

#include <memory>
#include<QRegularExpression>

// 注意ui里面的名字，应该算是一个对象（而不是一个类，以后规范代码）
RegisterDialog::~RegisterDialog() = default;



RegisterDialog::RegisterDialog(QWidget *parent): QDialog(parent), ui(std::make_unique<Ui::RegisterDialog>()){
	ui->setupUi(this);
	// 虽然很多的操作可以在 designer 中设置， 但是在代码中更： 显而易见
	
	// 为密码的输入设置模式
	ui->pass_edit->setEchoMode(QLineEdit::Password);
	ui->confirm_edit->setEchoMode(QLineEdit::Password);


	// 收到信号： 通知要注册 
	connect(HttpMgr::GetInstance().get(),&HttpMgr::sig_reg_mod_finished,
	this, &RegisterDialog::slot_reg_mod_finished);
	
	// 初始化 http 的回调函数
	initHttpHandlers();

    //day11 设定输入框输入后清空字符串 (错误提示)
    ui->err_tip->clear();
    connect(ui->user_edit,&QLineEdit::editingFinished,this,[this](){
        checkUserValid();
    });
    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this](){
        checkEmailValid();
    });
    connect(ui->pass_edit, &QLineEdit::editingFinished, this, [this](){
        checkPassValid();
    });
    connect(ui->confirm_edit, &QLineEdit::editingFinished, this, [this](){
        checkConfirmValid();
    });
    connect(ui->verify_edit, &QLineEdit::editingFinished, this, [this](){
            checkVarifyValid();
    });


	// 密码显示隐藏
	//设置浮动显示手形状
	ui->pass_visible->setCursor(Qt::PointingHandCursor);
	ui->confirm_visible->setCursor(Qt::PointingHandCursor);
	ui->pass_visible->SetState("unvisible", "unvisible_hover", "" ,"visible","visible_hover","");
	ui->confirm_visible->SetState("unvisible" ,"unvisible_hover" ,"" ,"visible" ,"visible_hover" ,"");
	//连接点击事件
	connect(ui->pass_visible, &ClickedLabel::clicked, this, [this]() {
		auto state = ui->pass_visible->GetCurState();
		if(state == ClickLbState::Normal){
			ui->pass_edit->setEchoMode(QLineEdit::Password);
		}else{
				ui->pass_edit->setEchoMode(QLineEdit::Normal);
		}
		qDebug() << "Label was clicked!";
	});
	connect(ui->confirm_visible, &ClickedLabel::clicked, this, [this]() {
		auto state = ui->confirm_visible->GetCurState();
		if(state == ClickLbState::Normal){
			ui->confirm_edit->setEchoMode(QLineEdit::Password);
		}else{
				ui->confirm_edit->setEchoMode(QLineEdit::Normal);
		}
		qDebug() << "Label was clicked!";
	});

	// 创建定时器（返回界面的倒计时）
	_countdown_timer = new QTimer(this);
	_countdown = 5;
	// 连接信号和槽
	connect(_countdown_timer, &QTimer::timeout, [this](){
		if(_countdown==0){
			_countdown_timer->stop();
			emit sigSwitchLogin();
			return;
		}
		_countdown--;
		auto str = QString("注册成功，%1 s后返回登录").arg(_countdown);
		ui->tip_lb->setText(str);
	});


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

	// 流程： PostHttpReq ---> 等待服务器回应 ---> 回调通知模块进行处理
	// 这个函数使用的前提： initHttphandlers , 就是得初始化这个回调函数
	_handlersMap[id](jsonDoc.object());
	return;
}

void RegisterDialog::initHttpHandlers(){

	// 注册: 获取验证码回包逻辑
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

	//注册: 注册用户回包逻辑
    _handlersMap.insert(ReqId::ID_REG_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
		if(error == ErrorCodes::UserExist){
			showTip(tr("用户或邮箱已存在"),false);
            return;
		}

        if(error != ErrorCodes::Success){
            showTip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("用户注册成功"), true);
        qDebug()<< "email is " << email ;
		qDebug()<< "user uuid is " <<  jsonObj["uuid"].toString();
		ChangeTipPage();
    });
}

void RegisterDialog::ChangeTipPage(){
    _countdown_timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);
    // 启动定时器，设置间隔为1000毫秒（1秒）
    _countdown_timer->start(1000);
}



void RegisterDialog::on_get_code_button_clicked(){
	auto email = ui->email_edit->text();
	// 对于这种多规则的东西的判读： 正则表达式
	// QRegularExpression regex("");
	QRegularExpression regex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
	bool match = regex.match(email).hasMatch();

	if(match){
		// 发送一个 http 的 get_verifycode 请求 --> 获取验证码
		QJsonObject jsonObj;
		jsonObj["email"] = email; 
		HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/get_verifycode"),jsonObj, ReqId::ID_GET_VARIFY_CODE,Modules::REGISTERMOD);


		// 错误处理在这里，就是接收 response 的时候处理 （这就是不同模块）
		//slot_reg_mod_finished

	}
	else{
		//提示邮箱不正确
		showTip(tr("邮箱地址不正确"),false);
	}

}
void RegisterDialog::on_sure_btn_clicked(){

    bool valid = checkUserValid();
    if(!valid){
        return;
    }
    valid = checkEmailValid();
    if(!valid){
        return;
    }
    valid = checkPassValid();
    if(!valid){
        return;
    }
    valid = checkVarifyValid();
    if(!valid){
        return;
    }

	// 发送http请求注册用户
    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = ui->pass_edit->text();
    json_obj["confirm"] = ui->confirm_edit->text();
    json_obj["verifycode"] = ui->verify_edit->text();

    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_register"),
	json_obj, ReqId::ID_REG_USER,Modules::REGISTERMOD);
	

}




void RegisterDialog::showTip(QString str,bool isOk){
	if(isOk){
		ui->err_tip->setProperty("state","normal");
	}
	else{
		ui->err_tip->setProperty("state","err");
	}
	ui->err_tip->setText(str);
	repolish(ui->err_tip);
}

bool RegisterDialog::checkUserValid()
{
    if(ui->user_edit->text() == ""){
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool RegisterDialog::checkEmailValid()
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_edit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(!match){
        //提示邮箱不正确
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }

    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool RegisterDialog::checkPassValid()
{
    auto pass = ui->pass_edit->text();
    auto confirm = ui->confirm_edit->text();

    if(pass.length() < 6 || pass.length()>15){
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*.]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);

    if(pass != confirm){
        //提示密码不匹配
        AddTipErr(TipErr::TIP_PWD_CONFIRM, tr("密码和确认密码不匹配"));
        return false;
    }else{
       DelTipErr(TipErr::TIP_PWD_CONFIRM);
    }
    return true;
}

bool RegisterDialog::checkVarifyValid()
{
    auto pass = ui->verify_edit->text();
    if(pass.isEmpty()){
        AddTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

bool RegisterDialog::checkConfirmValid()
{
    auto pass = ui->pass_edit->text();
    auto confirm = ui->confirm_edit->text();

    if(confirm.length() < 6 || confirm.length() > 15 ){
        //提示长度不准确
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*.]{6,15}$");
    bool match = regExp.match(confirm).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("不能包含非法字符"));
        return false;
    }

    DelTipErr(TipErr::TIP_CONFIRM_ERR);

    if(pass != confirm){
        //提示密码不匹配
        AddTipErr(TipErr::TIP_PWD_CONFIRM, tr("确认密码和密码不匹配"));
        return false;
    }else{
       DelTipErr(TipErr::TIP_PWD_CONFIRM);
    }
    return true;
}

void RegisterDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void RegisterDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
        if(_tip_errs.empty()){
            ui->err_tip->clear();
      return;
    }

    showTip(_tip_errs.first(), false);
}

void RegisterDialog::on_return_btn_clicked(){
	_countdown_timer->stop();
	emit sigSwitchLogin();
}

void RegisterDialog::on_cancel_btn_clicked(){
	_countdown_timer->stop();
	emit sigSwitchLogin();
}