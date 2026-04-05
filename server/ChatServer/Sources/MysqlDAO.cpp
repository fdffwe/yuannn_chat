#include "MysqlDAO.hpp"
#include "ConfigMgr.hpp"

MysqlDao::MysqlDao(){
    auto & cfg = ConfigMgr::GetInstance();
    const auto& host = cfg["MysqlServer"]["host"];
    const auto& port = cfg["MysqlServer"]["port"];
    const auto& pwd = cfg["MysqlServer"]["passwd"];
    const auto& schema = cfg["MysqlServer"]["schema"];
    const auto& user = cfg["MysqlServer"]["user"];
    pool_.reset(new MySqlPool(host+":"+port, user, pwd,schema, 5));
}

MysqlDao::~MysqlDao(){
    pool_->Close();
}

int MysqlDao::RegUser(const std::string& name, const std::string& email, const std::string& pwd){
    auto con = pool_->getConnection();
    try {
        if (con == nullptr) {
            return -1;
        }

        std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement("CALL reg_user(?,?,?,@result)"));
        stmt->setString(1, name);
        stmt->setString(2, email);
        stmt->setString(3, pwd);
        stmt->execute();

        std::unique_ptr<sql::Statement> stmtResult(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @result AS result"));
        if (res->next()) {
            int result = res->getInt("result");
            pool_->returnConnection(std::move(con));
            return result;
        }
        pool_->returnConnection(std::move(con));
        return -1;
    }
    catch (sql::SQLException& e) {
        if (con) pool_->returnConnection(std::move(con));
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return -1;
    }
}

bool MysqlDao::CheckEmail(const std::string& name, const std::string& email) {
    auto con = pool_->getConnection();
    try {
        if (con == nullptr) {
            pool_->returnConnection(std::move(con));
            return false;
        }
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT email FROM user WHERE name = ?"));
        pstmt->setString(1, name);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if(res->next()) {
            if (email != res->getString("email")) {
                pool_->returnConnection(std::move(con));
                return false;
            }
            pool_->returnConnection(std::move(con));
            return true;
        }
        pool_->returnConnection(std::move(con));
        return false;
    }
    catch (sql::SQLException& e) {
        if (con) pool_->returnConnection(std::move(con));
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

bool MysqlDao::UpdatePwd(const std::string& name, const std::string& newpwd) {
    auto con = pool_->getConnection();
    try {
        if (con == nullptr) {
            pool_->returnConnection(std::move(con));
            return false;
        }
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE user SET pwd = ? WHERE name = ?"));
        pstmt->setString(1, newpwd);
        pstmt->setString(2, name);
        int updateCount = pstmt->executeUpdate();
        pool_->returnConnection(std::move(con));
        return updateCount >= 0;
    }
    catch (sql::SQLException& e) {
        if (con) pool_->returnConnection(std::move(con));
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

bool MysqlDao::CheckPwd(const std::string& name, const std::string& pwd, UserInfo& userInfo) {
    auto con = pool_->getConnection();
    Defer defer([this, &con]() { pool_->returnConnection(std::move(con)); });
    try {
        if (con == nullptr) return false;
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT * FROM user WHERE name = ?"));
        pstmt->setString(1, name);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        std::string origin_pwd = "";
        while (res->next()) {
            origin_pwd = res->getString("pwd");
            break;
        }
        if (pwd != origin_pwd) return false;
        userInfo.name = name;
        userInfo.email = res->getString("email");
        userInfo.uid = res->getInt("uid");
        userInfo.pwd = origin_pwd;
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

std::shared_ptr<UserInfo> MysqlDao::GetUser(int uid)
{
    auto con = pool_->getConnection();
    if (con == nullptr) return nullptr;
    Defer defer([this, &con]() { pool_->returnConnection(std::move(con)); });
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT * FROM user WHERE uid = ?"));
        pstmt->setInt(1, uid);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        std::shared_ptr<UserInfo> user_ptr = nullptr;
        while (res->next()) {
            user_ptr.reset(new UserInfo);
            user_ptr->pwd = res->getString("pwd");
            user_ptr->email = res->getString("email");
            user_ptr->name= res->getString("name");
            user_ptr->nick = res->getString("nick");
            user_ptr->desc = res->getString("desc");
            user_ptr->sex = res->getInt("sex");
            user_ptr->icon = res->getString("icon");
            user_ptr->uid = uid;
            break;
        }
        return user_ptr;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return nullptr;
    }
}

std::shared_ptr<UserInfo> MysqlDao::GetUser(std::string name)
{
    auto con = pool_->getConnection();
    if (con == nullptr) return nullptr;
    Defer defer([this, &con]() { pool_->returnConnection(std::move(con)); });
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT * FROM user WHERE name = ?"));
        pstmt->setString(1, name);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        std::shared_ptr<UserInfo> user_ptr = nullptr;
        while (res->next()) {
            user_ptr.reset(new UserInfo);
            user_ptr->pwd = res->getString("pwd");
            user_ptr->email = res->getString("email");
            user_ptr->name = res->getString("name");
            user_ptr->nick = res->getString("nick");
            user_ptr->desc = res->getString("desc");
            user_ptr->sex = res->getInt("sex");
            user_ptr->uid = res->getInt("uid");
            break;
        }
        return user_ptr;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return nullptr;
    }
}

bool MysqlDao::GetMessages(int thread_id, long long since_id, int limit, std::vector<DbMessage>& out) {
    auto con = pool_->getConnection();
    if (con == nullptr) {
        return false;
    }

    Defer defer([this, &con]() { pool_->returnConnection(std::move(con)); });

    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
            "SELECT message_id, thread_id, fromuid, touid, content, UNIX_TIMESTAMP(created_at)*1000 AS created_at "
            "FROM message WHERE thread_id = ? AND message_id > ? ORDER BY message_id ASC LIMIT ?"));

        pstmt->setInt(1, thread_id);
        pstmt->setInt64(2, since_id);
        pstmt->setInt(3, limit);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        while (res->next()) {
            DbMessage m;
            m.message_id = res->getInt64("message_id");
            m.thread_id = res->getInt("thread_id");
            m.fromuid = res->getInt("fromuid");
            m.touid = res->getInt("touid");
            try { m.content = res->getString("content"); } catch(...) { m.content = ""; }
            m.created_at = res->getInt64("created_at");
            out.push_back(std::move(m));
        }
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

bool MysqlDao::GetMessagesForUser(int to_uid, long long since_id, int limit, std::vector<DbMessage>& out) {
    auto con = pool_->getConnection();
    if (con == nullptr) {
        return false;
    }

    Defer defer([this, &con]() { pool_->returnConnection(std::move(con)); });

    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
            "SELECT message_id, thread_id, fromuid, touid, content, UNIX_TIMESTAMP(created_at)*1000 AS created_at "
            "FROM message WHERE (touid = ? OR fromuid = ?) AND message_id > ? ORDER BY message_id ASC LIMIT ?"));

        pstmt->setInt(1, to_uid);
        pstmt->setInt(2, to_uid);
        pstmt->setInt64(3, since_id);
        pstmt->setInt(4, limit);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        while (res->next()) {
            DbMessage m;
            m.message_id = res->getInt64("message_id");
            m.thread_id = res->getInt("thread_id");
            m.fromuid = res->getInt("fromuid");
            m.touid = res->getInt("touid");
            try { m.content = res->getString("content"); } catch(...) { m.content = ""; }
            m.created_at = res->getInt64("created_at");
            out.push_back(std::move(m));
        }
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

long long MysqlDao::GetPrivateThread(int user1_id, int user2_id) {
    // normalize order: smaller id first to match unique constraint
    long long a = std::min((long long)user1_id, (long long)user2_id);
    long long b = std::max((long long)user1_id, (long long)user2_id);
    auto con = pool_->getConnection();
    if (con == nullptr) return -1;
    Defer defer([this, &con]() { pool_->returnConnection(std::move(con)); });
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
            "SELECT thread_id FROM private_chat WHERE user1_id = ? AND user2_id = ? LIMIT 1"));
        pstmt->setUInt64(1, a);
        pstmt->setUInt64(2, b);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            return res->getInt64("thread_id");
        }
        return -1;
    } catch (sql::SQLException& e) {
        std::cerr << "SQLException: " << e.what() << std::endl;
        return -1;
    }
}

long long MysqlDao::CreatePrivateThread(int user1_id, int user2_id) {
    long long a = std::min((long long)user1_id, (long long)user2_id);
    long long b = std::max((long long)user1_id, (long long)user2_id);
    auto con = pool_->getConnection();
    if (con == nullptr) return -1;
    try {
        Defer defer([this, &con]() { pool_->returnConnection(std::move(con)); });
        // start transaction
        con->setAutoCommit(false);
        // insert thread
        std::unique_ptr<sql::PreparedStatement> insertThread(con->prepareStatement(
            "INSERT INTO chat_thread (`type`) VALUES ('private')"));
        insertThread->executeUpdate();
        // get last insert id
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery("SELECT LAST_INSERT_ID() AS id"));
        long long thread_id = -1;
        if (rs->next()) thread_id = rs->getInt64("id");
        if (thread_id <= 0) {
            con->rollback();
            con->setAutoCommit(true);
            return -1;
        }
        // insert private_chat mapping (may fail on duplicate)
        try {
            std::unique_ptr<sql::PreparedStatement> insertMap(con->prepareStatement(
                "INSERT INTO private_chat (thread_id, user1_id, user2_id) VALUES (?, ?, ?)") );
            insertMap->setUInt64(1, thread_id);
            insertMap->setUInt64(2, a);
            insertMap->setUInt64(3, b);
            insertMap->executeUpdate();
            con->commit();
            con->setAutoCommit(true);
            return thread_id;
        } catch (sql::SQLException& e) {
            // duplicate key or other error: try to find existing mapping
            try { con->rollback(); } catch(...){}
            con->setAutoCommit(true);
            long long exist = GetPrivateThread((int)a, (int)b);
            if (exist > 0) return exist;
            return -1;
        }
    } catch (sql::SQLException& e) {
        if (con) pool_->returnConnection(std::move(con));
        std::cerr << "SQLException: " << e.what() << std::endl;
        return -1;
    }
}

bool MysqlDao::GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit) {
    auto con = pool_->getConnection();
    if (con == nullptr) return false;
    Defer defer([this, &con]() { pool_->returnConnection(std::move(con)); });
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("select apply.from_uid, apply.status, user.name, "
            "user.nick, user.sex from friend_apply as apply join user on apply.from_uid = user.uid where apply.to_uid = ? "
            "and apply.id > ? order by apply.id ASC LIMIT ? "));
        pstmt->setInt(1, touid);
        pstmt->setInt(2, begin);
        pstmt->setInt(3, limit);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        while (res->next()) {
            auto name = res->getString("name");
            auto uid = res->getInt("from_uid");
            auto status = res->getInt("status");
            auto nick = res->getString("nick");
            auto sex = res->getInt("sex");
            auto apply_ptr = std::make_shared<ApplyInfo>(uid, name, "", "", nick, sex, status);
            applyList.push_back(apply_ptr);
        }
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

bool MysqlDao::GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo> >& user_info_list) {
    auto con = pool_->getConnection();
    if (con == nullptr) return false;
    Defer defer([this, &con]() { pool_->returnConnection(std::move(con)); });
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("select * from friend where self_id = ? "));
        pstmt->setInt(1, self_id);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        while (res->next()) {
            auto friend_id = res->getInt("friend_id");
            auto back = res->getString("back");
            auto user_info = GetUser(friend_id);
            if (user_info == nullptr) continue;
            user_info->back = back;
            user_info_list.push_back(user_info);
        }
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

bool MysqlDao::AddFriendApply(const int& from, const int& to, const std::string& back_name)
{
    auto con = pool_->getConnection();
    if (con == nullptr) return false;
    Defer defer([this, &con]() { pool_->returnConnection(std::move(con)); });
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT INTO friend_apply (from_uid, to_uid) values (?,?) "
            "ON DUPLICATE KEY UPDATE from_uid = from_uid, to_uid = to_uid"));
        pstmt->setInt(1, from);
        pstmt->setInt(2, to);
        int rowAffected = pstmt->executeUpdate();
        return rowAffected >= 0;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

bool MysqlDao::AuthFriendApply(const int& from, const int& to) {
    auto con = pool_->getConnection();
    if (con == nullptr) return false;
    Defer defer([this, &con]() { pool_->returnConnection(std::move(con)); });
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE friend_apply SET status = 1 "
            "WHERE from_uid = ? AND to_uid = ?"));
        pstmt->setInt(1, to);
        pstmt->setInt(2, from);
        int rowAffected = pstmt->executeUpdate();
        return rowAffected >= 0;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

bool MysqlDao::AddFriend(const int& from, const int& to, const std::string& applicant_back, const std::string& acceptor_back) {
    auto con = pool_->getConnection();
    if (con == nullptr) return false;
    Defer defer([this, &con]() { pool_->returnConnection(std::move(con)); });
    try {
        con->setAutoCommit(false);
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT IGNORE INTO friend(self_id, friend_id, back) VALUES (?, ?, ?)"));
        pstmt->setInt(1, from);
        pstmt->setInt(2, to);
        pstmt->setString(3, applicant_back);
        pstmt->executeUpdate();

        std::unique_ptr<sql::PreparedStatement> pstmt2(con->prepareStatement("INSERT IGNORE INTO friend(self_id, friend_id, back) VALUES (?, ?, ?)"));
        pstmt2->setInt(1, to);
        pstmt2->setInt(2, from);
        pstmt2->setString(3, acceptor_back);
        pstmt2->executeUpdate();

        con->commit();
        con->setAutoCommit(true);
        return true;
    }
    catch (sql::SQLException& e) {
        try { if (con) con->rollback(); } catch(...){}
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

bool MysqlDao::AddChatMsg(long long thread_id, int fromuid, int touid, const std::string &content, long long &out_message_id) {
    auto con = pool_->getConnection();
    if (con == nullptr) return false;
    Defer defer([this, &con]() { pool_->returnConnection(std::move(con)); });
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
            "INSERT INTO message (thread_id, fromuid, touid, content, created_at) VALUES (?, ?, ?, ?, NOW())"));
        pstmt->setUInt64(1, (uint64_t)thread_id);
        pstmt->setUInt(2, fromuid);
        pstmt->setUInt(3, touid);
        pstmt->setString(4, content);
        pstmt->executeUpdate();

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery("SELECT LAST_INSERT_ID() AS id"));
        if (rs->next()) {
            out_message_id = rs->getInt64("id");
            return true;
        }
        return false;
    } catch (sql::SQLException &e) {
        std::cerr << "AddChatMsg SQLException: " << e.what() << std::endl;
        return false;
    }
}
