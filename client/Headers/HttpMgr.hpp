#pragma once
#include"Singleton.hpp"
#include"Global.hpp"

// Qobject 必须放在第一个 MOC 需要的（底层实现是如此的）

class HttpMgr : public QObject, public Singleton<HttpMgr>, public std::enable_shared_from_this<HttpMgr>{
    Q_OBJECT
    friend class Singleton<HttpMgr>;

public: 
    // 为什么public:  智能指针管理的时候，会调用这个析构： 需要权限的
    ~HttpMgr();

    // url：服务器地址   json:信息   后面两个是为了：透传（知道上次请求的信息，因为是异步的）
    // ???异步,  回调 （这里的回调， 哪里来的） ???
    void PostHttpReq(QUrl url, QJsonObject json, ReqId reqId, Modules mod); 


private: 
    HttpMgr();
    QNetworkAccessManager _manager;

// Qt 的异步机制
private slots: 
    void slot_http_finished(ReqId id ,QString res, ErrorCodes err, Modules mod);
    

// 说是谁 发送 信号， 谁定义 信号
// 说是谁 接收 信号， 谁定义 槽

signals: 

    /**
     * @brief Http请求的回调函数，发送请求的时候：消息透传了
     * @param res 是服务器的返回的结构： 比如请求登录，服务器返回：成功/失败（Json字符串）
     * @note 回调时候，好知道自己是要干嘛
     */
    void sig_http_finished(ReqId id ,QString res, ErrorCodes err, Modules mod); 
    
    void sig_reg_mod_finished(ReqId id ,QString res, ErrorCodes err); 
    void sig_reset_mod_finish(ReqId id, QString res, ErrorCodes err);
    void sig_login_mod_finish(ReqId id, QString res, ErrorCodes err);
};