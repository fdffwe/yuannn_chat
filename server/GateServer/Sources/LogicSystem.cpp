#include "LogicSystem.hpp"
#include "HttpConnection.hpp"
#include "VerifyGrpcClient.hpp"
#include "RedisMgr.hpp"
#include "MysqlMgr.hpp"

LogicSystem::LogicSystem(){

    // 注册 GET
    RegGet("/get_test",[](std::shared_ptr<HttpConnection> connection){
        
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
    RegPost("/get_verifycode",[](std::shared_ptr<HttpConnection> connection){
        auto strBody = beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive data is " << strBody << std::endl;

        connection->_response.set(http::field::content_type, "application/json");
        Json::Value root, srcRoot;
        Json::Reader reader;

        bool parseSuccess = reader.parse(strBody, srcRoot);
        if (!parseSuccess) {
            std::cout << "failed to parse Json data! " << std::endl;
            root["error"] = ErrorCodes::Error_Json;
            beast::ostream(connection->_response.body()) << root.toStyledString();
            return;
        }

        std::string email = srcRoot["email"].asString();
        std::cout << "email is " << email << std::endl;

        // 调用 gRPC 客户端获取验证码
        GetVerifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVerifyCode(email);

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

    RegPost("/user_register", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        connection->_response.set(http::field::content_type, "text/json");
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;
        bool parse_success = reader.parse(body_str, src_root);
        if (!parse_success) {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_Json;
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
        bool b_get_varify = RedisMgr::GetInstance()->Get(CODEPREFIX+src_root["email"].asString(), varify_code);
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
        int uid = MysqlMgr::GetInstance()->RegUser(name, email, pwd);
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


    //重置回调逻辑
    RegPost("/reset_pwd", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        connection->_response.set(http::field::content_type, "text/json");
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;
        bool parse_success = reader.parse(body_str, src_root);
        if (!parse_success) {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }
        auto email = src_root["email"].asString();
        auto name = src_root["user"].asString();
        auto pwd = src_root["passwd"].asString();
        
        //先查找redis中email对应的验证码是否合理
        std::string  varify_code;
        bool b_get_varify = RedisMgr::GetInstance()->Get(CODEPREFIX + src_root["email"].asString(), varify_code);
        if (!b_get_varify) {
            std::cout << " get varify code expired" << std::endl;
            root["error"] = ErrorCodes::VarifyExpired;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }
        if (varify_code != src_root["varifycode"].asString()) {
            std::cout << " varify code error" << std::endl;
            root["error"] = ErrorCodes::VarifyCodeErr;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        //查询数据库判断用户名和邮箱是否匹配
        bool email_valid = MysqlMgr::GetInstance()->CheckEmail(name, email);
        if (!email_valid) {
            std::cout << " user email not match" << std::endl;
            root["error"] = ErrorCodes::EmailNotMatch;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }
        //更新密码为最新密码
        bool b_up = MysqlMgr::GetInstance()->UpdatePwd(name, pwd);
        if (!b_up) {
            std::cout << " update pwd failed" << std::endl;
            root["error"] = ErrorCodes::PasswdUpFailed;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }
        std::cout << "succeed to update password" << pwd << std::endl;
        root["error"] = 0;
        root["email"] = email;
        root["user"] = name;
        root["passwd"] = pwd;
        root["varifycode"] = src_root["varifycode"].asString();
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

void LogicSystem::RegGet(std::string url, HttpHandler handler){
    _getHandlers.insert({url,handler});
}


// POST
bool LogicSystem::handlePost(std::string url, std::shared_ptr<HttpConnection> connection){
    if(!_postHandlers.count(url)) return false;

    _postHandlers[url](connection);
    return true; 
}


void LogicSystem::RegPost(std::string url, HttpHandler handler){
    _postHandlers.insert({url,handler});
}




LogicSystem::~LogicSystem(){

}