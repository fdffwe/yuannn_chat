#pragma once 
// beast 网络库
#include<boost/asio.hpp>
#include<boost/beast.hpp>
#include<boost/beast/http.hpp>
// C++自带库
#include<memory>
#include<iostream>
#include<atomic> 
#include<condition_variable> 
#include<mutex> 
#include<queue>
// jsoncpp
#include<json/json.h>




namespace beast = boost::beast; 
namespace http = boost::beast::http;
namespace net = boost::asio; 
using tcp = boost::asio::ip::tcp; 


enum ErrorCodes{
    SUCCESS = 0, 
    Error_Json = 1001,  // JSON 解析错误
    RPC_FAILED = 1002,  // RPC 请求错误
    VarifyExpired = 1003, //验证码过期
	VarifyCodeErr = 1004, //验证码错误
	UserExist = 1005,       //用户已经存在
	PasswdErr = 1006,    //密码错误
	EmailNotMatch = 1007,  //邮箱不匹配
	PasswdUpFailed = 1008,  //更新密码失败
	PasswdInvalid = 1009,   //密码更新失败
	TokenInvalid = 1010,   //Token失效
	UidInvalid = 1011,  //uid无效

	MysqlError = 1100, // 数据库操作错误
};

class ConfigMgr;
extern ConfigMgr& gCfgMgr;


#define CODEPREFIX  "code_"