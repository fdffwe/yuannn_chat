#pragma once
#include <unordered_map>
#include <memory>
#include <mutex>

#include "Singleton.hpp"
class CSession;

// 因为我们用户登录后，要将连接(session)和用户uid绑定。为以后登陆踢人做准备。所以新增UserMgr管理类；

class UserMgr: public Singleton<UserMgr>
{
	friend class Singleton<UserMgr>;
public:
	~UserMgr();
	std::shared_ptr<CSession> GetSession(int uid);
	void SetUserSession(int uid, std::shared_ptr<CSession> session);
	void RmvUserSession(int uid, std::string session_id);
private:
	UserMgr();
	std::mutex _session_mtx;
	std::unordered_map<int, std::shared_ptr<CSession>> _uid_to_session;
};

