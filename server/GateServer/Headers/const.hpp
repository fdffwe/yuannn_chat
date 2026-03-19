#pragma once 

#include<boost/asio.hpp>
#include<boost/beast.hpp>
#include<boost/beast/http.hpp>

#include<memory>
#include<iostream>
#include<atomic> 
#include<condition_variable> 
#include<mutex> 
#include<queue>

#include<json/json.h>



namespace beast = boost::beast; 
namespace http = boost::beast::http;
namespace net = boost::asio; 
using tcp = boost::asio::ip::tcp; 


enum ErrorCodes{
    SUCCESS = 0, 
    ERROR_JSON = 1001,  // JSON 解析错误
    RPC_FAILED = 1002,  // RPC 请求错误
    
};

class ConfigMgr;
extern ConfigMgr& gCfgMgr;