#include "LogicSystem.hpp"
#include "HttpConnection.hpp"
#include "VerifyGrpcClient.hpp"
#include "RedisMgr.hpp"
#include "MysqlMgr.hpp"

LogicSystem::LogicSystem(){

    // 注册 GET
    regGet("/get_test",[](std::shared_ptr<HttpConnection> connection){
        
        beast::ostream(connection->_response.body()) << "receive get_test req" << std::endl;
        // beast::ostream(connection->_response.body()) << connection->_getParams.size();

        // url 参数解析
        int id = 0; 
        for(auto& elm : connection->_getParams){
            id++;
            beast::ostream(connection->_response.body()) << "param " << id << " key is " << elm.first << std::endl;  
            beast::ostream(connection->_response.body()) << "param " << id << " value is " << elm.second << std::endl;  
        }

    });

    // 注册 POST - 获取验证码
    regPost("/get_verifycode",[](std::shared_ptr<HttpConnection> connection){
        auto strBody = beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive data is " << strBody << std::endl;

        connection->_response.set(http::field::content_type, "application/json");
        Json::Value root, srcRoot;
        Json::Reader reader;

        bool parseSuccess = reader.parse(strBody, srcRoot);
        if (!parseSuccess) {
            std::cout << "failed to parse Json data! " << std::endl;
            root["error"] = ErrorCodes::ERROR_JSON;
            beast::ostream(connection->_response.body()) << root.toStyledString();
            return;
        }

        std::string email = srcRoot["email"].asString();
        std::cout << "email is " << email << std::endl;

        // 调用 gRPC 客户端获取验证码
        GetVerifyRsp rsp = VerifyGrpcClient::getInstance()->GetVerifyCode(email);

        root["error"] = rsp.error();
        root["email"] = rsp.email();
        root["code"] = rsp.code();

        beast::ostream(connection->_response.body()) << root.toStyledString() << std::endl;
    });

    // 注册 POST - 注册账号逻辑
    // 逻辑： client 向 gateServer 发送 get_verify 请求
    // gateServer 通过 grpc 向 VerifyServer 通信， 向 email 发送验证嘛
    // VerifyServer 发送验证码，并把 email 和 验证码 存入 mysql 中
    // 我们采用的是： 1
    // 1.GateServer 直接读取 redis
    // 2.VerifyServer 验证

    regPost("/user_register", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        connection->_response.set(http::field::content_type, "text/json");
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;
        bool parse_success = reader.parse(body_str, src_root);
        if (!parse_success) {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::ERROR_JSON;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }
        auto email = src_root["email"].asString();
        auto name = src_root["user"].asString();
        auto pwd = src_root["passwd"].asString();
        auto confirm = src_root["confirm"].asString();
        if (pwd != confirm) {
            std::cout << "password err " << std::endl;
            root["error"] = ErrorCodes::PasswdErr;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }
        //先查找redis中email对应的验证码是否合理
        std::string  varify_code;
        // 为什么这里发生错误了， 第一次有返回结果， 第二次没有 （并且一次返回 5 个）
        // 因为 redis 是没有密码的， auth 就会报错
        bool b_get_varify = RedisMgr::getInstance()->Get(CODEPREFIX+src_root["email"].asString(), varify_code);
        if (!b_get_varify) {
            std::cout << " get varify code expired" << std::endl;
            root["error"] = ErrorCodes::VarifyExpired;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        if (varify_code != src_root["verifycode"].asString()) {
            std::cout << " varify code error" << std::endl;
            root["error"] = ErrorCodes::VarifyCodeErr;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        // 查找数据库判断用户是否存在
        // 这个就是往数据库： 插入数据
        int uid = MysqlMgr::getInstance()->RegUser(name, email, pwd);
        if(uid == - 1){
            std::cout << "mysql error" << std::endl;
            root["error"] = ErrorCodes::MysqlError;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }
        if (uid == 0 ) {
            std::cout << " user or email exist" << std::endl;
            root["error"] = ErrorCodes::UserExist;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        root["error"] = 0;
        root["uid"] = uid;
        root["email"] = email;
        root ["user"]= name;
        root["passwd"] = pwd;
        root["confirm"] = confirm;
        root["verifycode"] = src_root["verifycode"].asString();
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;
    });

}



// GET
bool LogicSystem::handleGet(std::string url, std::shared_ptr<HttpConnection> connection){

    if(!_getHandlers.count(url)) return false; 
    _getHandlers[url](connection);

    return true; 
}

void LogicSystem::regGet(std::string url, HttpHandler handler){
    _getHandlers.insert({url,handler});
}


// POST
bool LogicSystem::handlePost(std::string url, std::shared_ptr<HttpConnection> connection){
    if(!_postHandlers.count(url)) return false;

    _postHandlers[url](connection);
    return true; 
}


void LogicSystem::regPost(std::string url, HttpHandler handler){
    _postHandlers.insert({url,handler});
}




LogicSystem::~LogicSystem(){

}