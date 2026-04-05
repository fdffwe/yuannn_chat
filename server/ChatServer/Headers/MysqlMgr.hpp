#pragma once 

#include "MysqlDAO.hpp"
#include "Singleton.hpp"
#include "const.hpp"

class MysqlMgr: public Singleton<MysqlMgr>{
    friend class Singleton<MysqlMgr>;
public:
    ~MysqlMgr();
    int RegUser(const std::string& name, const std::string& email,  const std::string& pwd);
    bool CheckEmail(const std::string& name, const std::string& email);
    bool UpdatePwd(const std::string& name, const std::string& pwd);
    bool CheckPwd(const std::string& name, const std::string& pwd, UserInfo& userInfo);
    std::shared_ptr<UserInfo> GetUser(int uid);
	std::shared_ptr<UserInfo> GetUser(std::string name);
    bool AddFriendApply(const int& from, const int& to, const std::string& back_name);
	bool AuthFriendApply(const int& from, const int& to);
    bool AddFriend(const int& from, const int& to, const std::string& applicant_back, const std::string& acceptor_back);
    bool GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit=10);
	bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo> >& user_info);
    bool GetMessages(int thread_id, long long since_id, int limit, std::vector<DbMessage>& out);
    bool GetMessagesForUser(int to_uid, long long since_id, int limit, std::vector<DbMessage>& out);
    long long GetPrivateThread(int user1_id, int user2_id);
    long long CreatePrivateThread(int user1_id, int user2_id);
    bool AddChatMsg(long long thread_id, int fromuid, int touid, const std::string &content, long long &out_message_id);

private:
    MysqlMgr();
    MysqlDao  _dao;
};