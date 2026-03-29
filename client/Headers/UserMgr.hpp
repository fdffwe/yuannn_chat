#pragma once 
#include <QObject>
#include <memory>
#include "Singleton.hpp"
#include "userdata.hpp"

class UserMgr:public QObject,public Singleton<UserMgr>,
        public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
public:
    friend class Singleton<UserMgr>;
    ~ UserMgr();
    void SetName(QString name);
    void SetUid(int uid);
    void SetToken(QString token);

    bool IsLoadChatFin();

private:
    UserMgr();
    QString _name;
    QString _token;
    int _uid;

    int _chat_loaded;
    int _contact_loaded;
    std::vector<std::shared_ptr<FriendInfo>> _friend_list;
};
