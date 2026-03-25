#pragma once

#include<QDialog>
#include <memory>
#include "ClickedLabel.hpp"


namespace Ui{ class LoginDialog; };


class Login : public QDialog {
    Q_OBJECT // 使用信号与槽的必要声明， 还不能有 ； 

public:
    // 得这样写啊， 因为写了一个参数，就没有无参构造了 ，  这样的效果还是可以等效的
    explicit Login(QWidget *parent = nullptr);
    ~Login();


public slots:
    void slot_forget_pwd();
    void on_login_btn_clicked();
    void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);
    void slot_tcp_con_finish(bool bsuccess);
    // void slot_login_failed(int);

signals:
    void switchRegister();
    void switchReset();
    void sig_connect_tcp(ServerInfo);


private:
    // 尝试智能指针： 自动析构， 不用自己管
    // share_ptr  unqiue_ptr
    // #include <memory>
    void initHttpHandlers();
    void showTip(QString str,bool b_ok);
    bool checkUserValid();
    bool checkPwdValid();



    std::unique_ptr<Ui::LoginDialog> ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    bool enableBtn(bool);
    QMap<TipErr, QString> _tip_errs;
    void AddTipErr(TipErr te,QString tips);
    void DelTipErr(TipErr te);
    int _uid;
    QString _token;
};