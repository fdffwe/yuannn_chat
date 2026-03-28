#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <boost/asio.hpp>

#include "const.hpp"
#include "ConfigMgr.hpp"
// #include "hiredis.h"
#include "RedisMgr.hpp"
#include "MysqlMgr.hpp"
// #include "AsioIOServicePool.hpp"
#include "StatusServiceImpl.hpp"


void RunServer() {
    auto & cfg = ConfigMgr::GetInstance(); 
    std::string server_address(cfg["StatusServer"]["Host"]+":"+ cfg["StatusServer"]["Port"]);
    StatusServiceImpl service;
    grpc::ServerBuilder builder;
    // 监听端口和添加服务
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    // 构建并启动gRPC服务器
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // 上面实现了 grpc 服务的监听， 只需等待 Client 调用

    // 创建Boost.Asio的io_context
    boost::asio::io_context io_context;
    // 创建signal_set用于捕获SIGINT
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    // 设置异步等待SIGINT信号
    signals.async_wait([&server](const boost::system::error_code& error, int signal_number) {
        if (!error) {
            std::cout << "Shutting down server..." << std::endl;
            server->Shutdown(); // 优雅地关闭服务器
        }
    });

    // 在单独的线程中运行io_context
    // io_context.run(); 会阻塞当前进程，所以放进一个新的线程中
    // io_context 的作用， 监听： 异步信号的 (epoll 模型)
    std::thread([&io_context]() { io_context.run(); }).detach();
    // 等待服务器关闭
    server->Wait();
    io_context.stop(); // 停止io_context
}


int main(int argc, char** argv) {
    try {
        RunServer();
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}