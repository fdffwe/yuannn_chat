#include "UserMgr.hpp"



UserMgr::~UserMgr()
{
}
void UserMgr::SetName(QString name)
{
    _name = name;
}
void UserMgr::SetUid(int uid)
{
    _uid = uid;
}
void UserMgr::SetToken(QString token)
{
    _token = token;
}
UserMgr::UserMgr()
{
}

bool UserMgr::IsLoadChatFin() {
    if (_chat_loaded >= _friend_list.size()) {
        return true;
    }

    return false;
}