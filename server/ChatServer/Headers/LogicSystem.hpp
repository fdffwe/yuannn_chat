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
	void SearchInfo(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	bool isPureDigit(const std::string& str);
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo> &userinfo);
	bool GetUserByUid(std::string uid_str, Json::Value& rtvalue);
	bool GetUserByName(std::string name, Json::Value& rtvalue);
	void AddFriendApply(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	void AuthFriendApply(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	bool GetFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list);
	bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>> & user_list);

	void DealChatTextMsg(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	
	void HeartBeatHandler(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
    void FetchMessagesHandler(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data);

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

