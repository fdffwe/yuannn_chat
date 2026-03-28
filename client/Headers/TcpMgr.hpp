#pragma once 
#include <QObject>
#include <QTcpSocket>
#include <QMap>
#include <QByteArray>
#include <functional>

#include "Singleton.hpp"
#include "Global.hpp"
#include "UserMgr.hpp"


class TcpMgr: public QObject, public Singleton<TcpMgr>,
    public std::enable_shared_from_this<TcpMgr> {
    Q_OBJECT
    friend class Singleton<TcpMgr>;
public:
    ~TcpMgr();

private:
    TcpMgr();
    QTcpSocket _socket;
    QString _host;
    uint16_t _port;
    QByteArray _buffer;
    bool _b_recv_pending;
    quint16 _message_id;
    quint16 _message_len;
    QMap<ReqId, std::function<void(ReqId, int, QByteArray)>> _handlers;

public slots:
    void slot_tcp_connect(ServerInfo);
    void slot_send_data(ReqId reqId, QString data);
    void initHandlers();
    void handleMsg(ReqId id, int len, QByteArray data);

signals:
    void sig_con_success(bool bsuccess);
    void sig_send_data(ReqId reqId, QString data);
    void sig_login_failed(int err);
    void sig_swich_chatdlg();

};