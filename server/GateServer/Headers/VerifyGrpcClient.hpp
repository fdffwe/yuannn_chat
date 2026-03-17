#pragma once

#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "Singleton.hpp"
#include "const.hpp"
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

// 作用（类）：   C++ 的 gateServer 和 Node.js 进行通信的专线


// 作用（Pool）:
// 程序刚启动时，我一次性提前建好 5 根专线（买 5 个充电宝放在机柜里）。
// 谁需要发邮件，就从池子里借一根（拿充电宝），发完邮件立刻还给池子（还充电宝）。这样就不需要频繁建立和销毁连接了，极大提升了并发性能！

// 典型： 生成者-消费模型
class RPCConPool {
public:
    
    RPCConPool(size_t poolSize, const std::string& host, const std::string& port)
        : _poolSize(poolSize), _host(host), _port(port), _b_stop(false) {

        for (size_t i = 0; i < poolSize; ++i) {
            auto channel = grpc::CreateChannel(host + ":" + port, grpc::InsecureChannelCredentials());
            _connections.push(VerifyService::NewStub(channel));
        }
    }

    ~RPCConPool() { close(); }

    std::unique_ptr<VerifyService::Stub> getConnection() {
        std::unique_lock<std::mutex> lock(_mutex);
        _conditions.wait(lock, [this] { return _b_stop || !_connections.empty(); });
        if (_b_stop) return nullptr;
        auto ctx = std::move(_connections.front());
        _connections.pop();
        return ctx;
    }

    void returnConnection(std::unique_ptr<VerifyService::Stub> context) {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_b_stop) return;
        _connections.push(std::move(context));
        _conditions.notify_one();
    }

    void close(){
        _b_stop = true; 
        // 有了上面的，告诉所有人退出了
        _conditions.notify_all();
    }


private:
    std::string _host;
    std::string _port;
    std::atomic<bool> _b_stop;
    size_t _poolSize;
    std::mutex _mutex;
    std::queue<std::unique_ptr<VerifyService::Stub>> _connections;
    std::condition_variable _conditions;
};


class VerifyGrpcClient : public Singleton<VerifyGrpcClient> {
    friend class Singleton<VerifyGrpcClient>;

public:
    GetVerifyRsp GetVerifyCode(const std::string& email) {
        ClientContext context;
        GetVerifyRsp reply;
        GetVerifyReq request;
        request.set_email(email);
        auto stub = pool_->getConnection();
        Status status = stub->GetVerifyCode(&context, request, &reply);
        if (status.ok()) {
            pool_->returnConnection(std::move(stub));
            return reply;
        }
        else {
            pool_->returnConnection(std::move(stub));
            reply.set_error(ErrorCodes::RPC_FAILED);
            return reply;
        }
    }

private:
    VerifyGrpcClient();
    std::unique_ptr<RPCConPool> pool_;
};
