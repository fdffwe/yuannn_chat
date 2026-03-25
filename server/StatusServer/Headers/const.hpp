#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "Singleton.hpp"
#include <assert.h>
#include <queue>

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>

#include <iostream>
#include <functional>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>


namespace beast = boost::beast; 
namespace http = boost::beast::http;
namespace net = boost::asio; 
using tcp = boost::asio::ip::tcp; 


enum ErrorCodes{
    Success = 0, 
    Error_Json = 1001,  // JSON 解析错误
    RPCFailed = 1002,  // RPC 请求错误
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


// 确保在对象离开作用域时，自动执行指定的清理函数，类似于智能指针的 RAII（Resource Acquisition Is Initialization）思想。
// 这个确实是非常的方便的， 不用自己一直 if else 然后：  回收资源了， 只需要 defer 一次即可
class Defer {
public:
	// 接受一个lambda表达式或者函数指针
	Defer(std::function<void()> func) : func_(func) {}

	// 析构函数中执行传入的函数
	~Defer() {
		func_();
	}

private:
	std::function<void()> func_;
};


#define CODEPREFIX  "code_"

