#include"HttpMgr.hpp"

HttpMgr::~HttpMgr(){

}

HttpMgr::HttpMgr(){

    connect(this,&HttpMgr::sig_http_finished,this,&HttpMgr::slot_http_finished);

}




void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId reqId, Modules mod){

    // Json 对象序列化： 字节数组
    QByteArray data = QJsonDocument(json).toJson();
    // 底层就是tcp, TLV 消息格式
    QNetworkRequest request(url);
    // 消息类型： 头
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    // 消息长度： 头（Qt 自动处理， 我不管了）

    
    // post 请求，注册回调函数
    // 这个异步呢是很危险的， 因为会调用 this（HttpMgr）
    // 为了防止崩溃， 我们得保证他的生命周期
    // 触发回调前， Http 得存在
    QNetworkReply* reply = _manager.post(request, data);
    auto self = shared_from_this();
    // 这个不是自定义的信号与槽： （所以用的 labmda）
    connect(reply,&QNetworkReply::finished,[self,reply,reqId,mod](){
        // 处理错误
        if(reply->error()){
            qDebug() << reply->errorString(); 
            // 发送信号通知完成 (告诉相关界面)
            emit self->sig_http_finished(reqId, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater(); // reply 可能用于底层事件循环，告诉系统不用再删
            return;
        }
        // 没有错误

        // 返回的信息
        QString res = reply->readAll();
        emit self->sig_http_finished(reqId, res, ErrorCodes::SUCCESS, mod);
        reply->deleteLater(); // reply 可能用于底层事件循环，告诉系统不用再删
        return;
    });
}

void HttpMgr::slot_http_finished(ReqId id ,QString res, ErrorCodes err, Modules mod){
    
    if(mod == Modules::REGISTERMOD){            
        // 发送信号通知指定模块Http响应结束
        emit sig_reg_mod_finished(id,res,err);
    } else if(mod == Modules::RESETMOD){
        // 重置密码模块的响应
        emit sig_reset_mod_finish(id, res, err);
    }
}

