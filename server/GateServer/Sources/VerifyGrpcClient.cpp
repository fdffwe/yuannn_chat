#include "VerifyGrpcClient.hpp"
#include "ConfigMgr.hpp"
#include "const.hpp"
#include <atomic>






VerifyGrpcClient::VerifyGrpcClient() {
    auto& gCfgMgr = ConfigMgr::GetInstance();
    std::string host = gCfgMgr["VerifyServer"]["host"];
    std::string port = gCfgMgr["VerifyServer"]["port"];
    pool_.reset(new RPCConPool(5, host, port));
}