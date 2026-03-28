#pragma once
#include <queue>
#include <thread>
#include <queue>
#include <map>
#include <functional>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <unordered_map>

#include "const.hpp"
#include "Singleton.hpp"
#include "data.hpp"

using namespace std;

class CSession; 
class LogicNode;

class CServer;
typedef  std::function<void(std::shared_ptr<CSession>, const short &msg_id, const string &msg_data)> FunCallBack;
class LogicSystem:public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem();
	void PostMsgToQue(std::shared_ptr <LogicNode> msg);
	void SetServer(std::shared_ptr<CServer> pserver);

	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo> &userinfo);
private:
	LogicSystem();
	void DealMsg();
	void RegisterCallBacks();
	void LoginHandler(std::shared_ptr<CSession> session, const short &msg_id, const string &msg_data);

	std::thread _worker_thread;
	std::queue<shared_ptr<LogicNode>> _msg_que;
	std::mutex _mutex;
	std::condition_variable _consume;
	bool _b_stop;
	std::map<short, FunCallBack> _fun_callbacks;
	std::shared_ptr<CServer> _p_server;
};

