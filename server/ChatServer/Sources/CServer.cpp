#include "CServer.hpp"
#include "CSession.hpp"
#include "UserMgr.hpp"
#include"RedisMgr.hpp"
#include"ConfigMgr.hpp"

CServer::CServer(boost::asio::io_context& io_context, short port):_io_context(io_context), _port(port),
_acceptor(io_context, tcp::endpoint(tcp::v4(),port)), _timer(_io_context, std::chrono::seconds(60)){
    cout << "Server start success, listen on port : " << _port << endl;
    StartAccept();
}

CServer::~CServer() {
	cout << "Server destruct listen on port : " << _port << endl;
}

void CServer::StartAccept() {
    auto &io_context = AsioIOServicePool::GetInstance()->GetIOService();
    shared_ptr<CSession> new_session = make_shared<CSession>(io_context, this);
    _acceptor.async_accept(new_session->GetSocket(), std::bind(&CServer::HandleAccept, this, new_session, placeholders::_1));
}


void CServer::HandleAccept(shared_ptr<CSession> new_session, const boost::system::error_code& error){
    if (!error) {
        new_session->Start();
        lock_guard<mutex> lock(_mutex);
        _sessions.insert(make_pair(new_session->GetSessionId(), new_session));
    }
    else {
        cout << "session accept failed, error is " << error.what() << endl;
    }
    StartAccept();
}


// 根据session 的id删除 session，并移除用户和session的关联
void CServer::ClearSession(std::string session_id) {
	
	lock_guard<mutex> lock(_mutex);
    if (_sessions.find(session_id) != _sessions.end()) {
        // 使用会话对象记录的用户 id，而不是 session id
        int uid = _sessions[session_id]->GetUserId();

        // 移除 用户 和 session 的关联
        UserMgr::GetInstance()->RmvUserSession(uid, session_id);
    }

	_sessions.erase(session_id);
}



void CServer::on_timer(const boost::system::error_code& e){
    if (e) return;
    // _expired_session 用来做缓存， 以至于可以先： 加分布式锁， 再加本地锁 ----> 对 _seesion 进行处理（避免死锁 和 资源竞争）
    std::vector<std::shared_ptr<CSession>> expired_list;
    int current_count = 0;
    time_t now = std::time(nullptr);
    //此处加锁遍历session
    {
        lock_guard<mutex> lock(_mutex);
        for (auto iter = _sessions.begin(); iter != _sessions.end(); ) {
            if (iter->second->IsHeartbeatExpired(now)) {
                // 1. 优雅地转移所有权到临时列表，不触发原子计数，性能最高
                expired_list.emplace_back(std::move(iter->second)); 
                // 2. 物理删除 map 中的节点
                iter = _sessions.erase(iter); 
            } 
            else {
                ++iter;
            }
        }
        current_count = _sessions.size(); // 实时获取当前在线人数
    }

    // 4. 在锁外执行耗时的操作
    // 负载上报
    auto& cfg = ConfigMgr::Inst();
    RedisMgr::GetInstance()->HSet(LOGIN_COUNT, cfg["SelfServer"]["Name"], std::to_string(current_count));

    // 处理异常 Session (清理 Redis、发送下线通知等)
    for (auto& session : expired_list) {
        session->Close(); // 关闭 Socket
        session->DealExceptionSession(); // 内部处理 Redis 锁释放、Uid 绑定清理等
    }

    // 5. 重新启动定时器
    _timer.expires_after(std::chrono::seconds(60));
    _timer.async_wait([this](const boost::system::error_code&ec) { on_timer(ec); });
}

void CServer::StartTimer()
{
	//启动定时器
	auto self(shared_from_this());
	_timer.async_wait([self](boost::system::error_code ec) {
		self->on_timer(ec);
    });
}

void CServer::StopTimer()
{
	_timer.cancel();
}

bool CServer::CheckValid(std::string uuid)
{
	lock_guard<mutex> lock(_mutex);
	auto it = _sessions.find(uuid);
	if (it != _sessions.end()) {
		return true;
	}
	return false;
}
