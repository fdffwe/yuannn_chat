#include"Server.hpp"
#include"HttpConnection.hpp"
#include "AsioIOServicePool.hpp"

#include<iostream>

Server::Server(net::io_context& ioc, unsigned short port): 
    _ioc(ioc),_acceptor(ioc,tcp::endpoint(tcp::v4(),port)){

}


void Server::start(){
    auto self = shared_from_this();

    // 服务池的调用
    auto& ioc = AsioIOServicePool::GetInstance()->GetIOService();
    auto newConnection = std::make_shared<HttpConnection>(ioc);


    // 必须捕获 newConnection 生命周期问题
    _acceptor.async_accept(newConnection->getSocket(),[self,newConnection](beast::error_code ec){
        try{
            // 这个是出错了，继续新连接
            if(ec){
                self->start(); 
                return; 
            }

            // 处理新连接
            newConnection->start();

            // 继续监听
            self->start(); 

        }
        catch(std::exception& e){
            std::cout << "exception is " << e.what() << std::endl; 
            self->start();
        }
    });
}