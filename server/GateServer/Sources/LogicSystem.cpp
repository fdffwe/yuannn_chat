#include"LogicSystem.hpp"
#include"HttpConnection.hpp"
#include"VarifyGrpcClient.hpp"



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
        GetVarifyRsp rsp = VerifyGrpcClient::getInstance()->GetVarifyCode(email);

        root["error"] = rsp.error();
        root["email"] = rsp.email();
        root["code"] = rsp.code();

        beast::ostream(connection->_response.body()) << root.toStyledString() << std::endl;
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