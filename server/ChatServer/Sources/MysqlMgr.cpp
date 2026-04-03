#include "MysqlMgr.hpp"

MysqlMgr::~MysqlMgr() {

}

int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd){
    return _dao.RegUser(name, email, pwd);
}


MysqlMgr::MysqlMgr() {

}

bool MysqlMgr::CheckEmail(const std::string& name, const std::string& email) {
    return _dao.CheckEmail(name, email);
}

bool MysqlMgr::UpdatePwd(const std::string& name, const std::string& pwd) {
    return _dao.UpdatePwd(name, pwd);
}

bool MysqlMgr::CheckPwd(const std::string& name, const std::string& pwd, UserInfo& userInfo) {
    return _dao.CheckPwd(name, pwd, userInfo);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(int uid)
{
	return _dao.GetUser(uid);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(std::string name)
{
	return _dao.GetUser(name);
}
bool MysqlMgr::AddFriendApply(const int& from, const int& to, const std::string& back_name)
{
	return _dao.AddFriendApply(from, to, back_name);
}

bool MysqlMgr::AuthFriendApply(const int& from, const int& to) {
	return _dao.AuthFriendApply(from, to);
}

bool MysqlMgr::AddFriend(const int& from, const int& to, const std::string& applicant_back, const std::string& acceptor_back) {
	return _dao.AddFriend(from, to, applicant_back, acceptor_back);
}
bool MysqlMgr::GetApplyList(int touid, 
	std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit) {

	return _dao.GetApplyList(touid, applyList, begin, limit);
}

bool MysqlMgr::GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo> >& user_info) {
	return _dao.GetFriendList(self_id, user_info);
}

bool MysqlMgr::GetMessages(int thread_id, long long since_id, int limit, std::vector<DbMessage>& out) {
    return _dao.GetMessages(thread_id, since_id, limit, out);
}

long long MysqlMgr::GetPrivateThread(int user1_id, int user2_id) {
	return _dao.GetPrivateThread(user1_id, user2_id);
}

long long MysqlMgr::CreatePrivateThread(int user1_id, int user2_id) {
	return _dao.CreatePrivateThread(user1_id, user2_id);
}