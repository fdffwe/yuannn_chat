#include "Login.hpp"
#include "ui_Login.h"
#include <memory>
#include "HttpMgr.hpp"
#include"TcpMgr.hpp"



Login::Login(QWidget *parent): QDialog(parent), ui(std::make_unique<Ui::LoginDialog>()){
	ui->setupUi(this);

	ui->pass_edit->setEchoMode(QLineEdit::Password);



	// 发送我要注册的信号， 由 MainWindow 统一管理
	connect(ui->registerButton, &QPushButton::clicked, this, &Login::switchRegister);


	ui->forget_label->SetState("normal","hover","","selected","selected_hover","");
    ui->forget_label->setCursor(Qt::PointingHandCursor);
    connect(ui->forget_label, &ClickedLabel::clicked, this, &Login::slot_forget_pwd);

	
	// 密码显示隐藏
	//设置浮动显示手形状
	ui->pass_visible->setCursor(Qt::PointingHandCursor);
	ui->pass_visible->SetState("unvisible", "unvisible_hover", "" ,"visible","visible_hover","");
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

	initHttpHandlers();

    //连接登录回包信号
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_login_mod_finish, this,&Login::slot_login_mod_finish);


    //连接tcp连接请求 的信号和槽函数
    connect(this, &Login::sig_connect_tcp, TcpMgr::GetInstance().get(), &TcpMgr::slot_tcp_connect);
    
    //连接tcp管理者 发出的连接成功信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_con_success, this, &Login::slot_tcp_con_finish);

    //连接tcp管理者发出的登陆失败信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_login_failed, this, &Login::slot_login_failed);

}

Login::~Login() = default;

void Login::slot_forget_pwd()
{
    qDebug()<<"slot forget pwd";
    emit switchReset();
}


void Login::on_login_btn_clicked()
{
    qDebug()<<"login btn clicked";
    if(checkUserValid() == false){
        return;
    }
    if(checkPwdValid() == false){
        return ;
    }
    auto user = ui->user_edit->text();
    auto pwd = ui->pass_edit->text();
    //发送http请求登录
    QJsonObject json_obj;
    json_obj["user"] = user;
    json_obj["passwd"] = QString(pwd);
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_login"),json_obj, ReqId::ID_LOGIN_USER,Modules::LOGINMOD);
}


bool Login::checkUserValid(){

    auto email = ui->user_edit->text();
    if(email.isEmpty()){
        qDebug() << "email empty " ;
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("用户名不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool Login::checkPwdValid(){
    auto pwd = ui->pass_edit->text();
    if(pwd.length() < 6 || pwd.length() > 15){
        qDebug() << "Pass length invalid";
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*.]{6,15}$");
    bool match = regExp.match(pwd).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符且长度为(6~15)"));
        return false;;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);

    return true;
}


void Login::initHttpHandlers()
{
    //注册获取登录回包逻辑
    _handlers.insert(ReqId::ID_LOGIN_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::Success){
            showTip(tr("参数错误"),false);
            enableBtn(true);
            return;
        }
        auto user = jsonObj["user"].toString();
        //发送信号通知tcpMgr发送长链接
        ServerInfo si;
        si.Uid = jsonObj["uid"].toInt();
        si.Host = jsonObj["host"].toString();
        si.Port = jsonObj["port"].toString();
        si.Token = jsonObj["token"].toString();
        _uid = si.Uid;
        _token = si.Token;
        qDebug()<< "user is " << user << " uid is " << si.Uid <<" host is "
            << si.Host << " Port is " << si.Port << " Token is " << si.Token;
        emit sig_connect_tcp(si);
    });
}

void Login::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::Success){
        showTip(tr("网络请求错误"),false);
        return;
    }
    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    //json解析错误
    if(jsonDoc.isNull()){
        showTip(tr("json解析错误"),false);
        return;
    }
    //json解析错误
    if(!jsonDoc.isObject()){
        showTip(tr("json解析错误"),false);
        return;
    }
    //调用对应的逻辑,根据id回调。
    _handlers[id](jsonDoc.object());
    return;
}


void Login::showTip(QString str, bool b_ok)
{
    if(b_ok){
         ui->err_tip->setProperty("state","normal");
    }else{
        ui->err_tip->setProperty("state","err");
    }

    ui->err_tip->setText(str);

    repolish(ui->err_tip);
}

void Login::AddTipErr(TipErr te,QString tips){
    _tip_errs[te] = tips;
    showTip(tips, false);
}
void Login::DelTipErr(TipErr te){
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
      ui->err_tip->clear();
      return;
    }

    showTip(_tip_errs.first(), false);
}


void Login::slot_tcp_con_finish(bool bsuccess)
{
   if(bsuccess){
      showTip(tr("聊天服务连接成功，正在登录..."),true);
      QJsonObject jsonObj;
      jsonObj["uid"] = _uid;
      jsonObj["token"] = _token;
      QJsonDocument doc(jsonObj);
      QString jsonString = doc.toJson(QJsonDocument::Indented);
      //发送tcp请求给chat server
      TcpMgr::GetInstance()->sig_send_data(ReqId::ID_CHAT_LOGIN, jsonString);
   }else{
      showTip(tr("网络异常"),false);
      enableBtn(true);
   }
}

bool Login::enableBtn(bool enabled)
{
    ui->login_btn->setEnabled(enabled);
    ui->registerButton->setEnabled(enabled);
    return true;
}

void Login::slot_login_failed(int err)
{
    QString result = QString("登录失败, err is %1")
                             .arg(err);
    showTip(result,false);
    enableBtn(true);
}