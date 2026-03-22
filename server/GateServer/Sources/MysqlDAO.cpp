#include "MysqlDAO.hpp"
#include "ConfigMgr.hpp"

MysqlDao::MysqlDao(){
    auto & cfg = ConfigMgr::getInstance();
    const auto& host = cfg["MysqlServer"]["host"];
    const auto& port = cfg["MysqlServer"]["port"];
    const auto& pwd = cfg["MysqlServer"]["passwd"];
    const auto& schema = cfg["MysqlServer"]["schema"];
    const auto& user = cfg["MysqlServer"]["user"];
    pool_.reset(new MySqlPool(host+":"+port, user, pwd,schema, 5));
}

MysqlDao::~MysqlDao(){
    // 类似 对象树 的机制， 再别的类里面，控制别人的生命周期
    pool_->Close();
}
  
int MysqlDao::RegUser(const std::string& name, const std::string& email, const std::string& pwd){
    auto con = pool_->getConnection();
    try {
        if (con == nullptr) {
            pool_->returnConnection(std::move(con));
            return false;
        }

        // 准备调用存储过程
        std::unique_ptr < sql::PreparedStatement > stmt(con->prepareStatement("CALL reg_user(?,?,?,@result)"));
        // 设置输入参数
        stmt->setString(1, name);
        stmt->setString(2, email);
        stmt->setString(3, pwd);

        // 由于PreparedStatement不直接支持注册输出参数，我们需要使用会话变量或其他方法来获取输出参数的值
        // 执行存储过程
        stmt->execute();

        // 如果存储过程设置了会话变量 或 有其他方式获取输出参数的值，你可以在这里执行 SELECT查询 来获取它们
        // 例如，如果存储过程设置了一个会话变量 @result 来存储输出结果，可以这样获取：
        std::unique_ptr<sql::Statement> stmtResult(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @result AS result"));
        if (res->next()) {
            int result = res->getInt("result");
            std::cout << "Result: " << result << std::endl;
            pool_->returnConnection(std::move(con));
            return result;
        }
        pool_->returnConnection(std::move(con));
        return -1;

    }

    
    catch (sql::SQLException& e) {
        pool_->returnConnection(std::move(con));
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return -1;
    }
}