#pragma once 
#include "Singleton.hpp"
#include "const.hpp"

#include<map>
#include<functional>


class HttpConnection;
typedef std::function< void(std::shared_ptr<HttpConnection>) > HttpHandler;


class LogicSystem : public Singleton<LogicSystem>{
    friend class Singleton<LogicSystem>;

public: 

    ~LogicSystem(); 

    void RegGet(std::string, HttpHandler);
    bool handleGet(std::string, std::shared_ptr<HttpConnection>);

    // 构造的时候， 注册对应：URL 操作
    void RegPost(std::string, HttpHandler);
    // 执行对应： URL 操作
    bool handlePost(std::string, std::shared_ptr<HttpConnection>);


private: 

    LogicSystem();
    
    // url string
    std::unordered_map<std::string,HttpHandler> _postHandlers;
    std::unordered_map<std::string,HttpHandler> _getHandlers;


};