#include "CServer.hpp"
#include "CSession.hpp"
#include "UserMgr.hpp"


CServer::CServer(boost::asio::io_context& io_context, short port):_io_context(io_context), _port(port),
_acceptor(io_context, tcp::endpoint(tcp::v4(),port)){
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
