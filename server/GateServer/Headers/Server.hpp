#pragma once 

#include"const.hpp"
#include"AsioIOServicePool.hpp"




class Server : public std::enable_shared_from_this<Server>{

public:
    Server(net::io_context& _ioc, unsigned short port);
    void start();


private:
    net::io_context& _ioc;
    tcp::acceptor _acceptor;
    

    // 这样子写是多余的， 完全可以放进 连接处理里面（每一个连接：都得有新的 _socket）
    // // 这个是给 HttpConnection 用的
    // tcp::socket _socket; 

};