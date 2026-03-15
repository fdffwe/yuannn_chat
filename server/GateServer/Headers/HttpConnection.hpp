#pragma once 
#include"const.hpp"


class HttpConnection : public std::enable_shared_from_this<HttpConnection>{

    // 这个服务器使用： 要用消息队列： 实现逻辑解耦
    friend class LogicSystem; 


public: 

    HttpConnection(net::io_context& ioc); 
    
    // 对消息的监听
    void start(); 

    // 给 Server 用的
    tcp::socket& getSocket();





private: 

    void handleReq();

    void writeResponse(); 

    void checkDeadline();

    // URL 解析
    void parseGetParam();
    

    // 网络IO
    tcp::socket _socket; 

    // 请求消息(这个一个头，一个buffer， 接收信息，自己构造 request )
    http::request<http::dynamic_body> _request;
    beast::flat_buffer _buffer{8192};

    // 回复（直接一个 response 解决）
    http::response<http::dynamic_body> _response; 

    // deadline 计时器
    net::steady_timer _deadline{_socket.get_executor(),std::chrono::seconds(60)};


    // URL 相关的（编码，解码）
    std::string _getUrl;
    std::unordered_map<std::string,std::string> _getParams;

};