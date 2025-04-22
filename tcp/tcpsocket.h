#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QObject>
#ifdef Q_OS_WIN
#define FD_SETSIZE 2048
#include <winsock2.h>
#endif

class TcpSocket : QObject
{
    Q_OBJECT

public:
    TcpSocket(QObject* parent = nullptr);
    TcpSocket(QByteArray ipAddress, unsigned short port, QObject* parent = nullptr);
    ~TcpSocket();

    bool connectToHost(QByteArray ipAddress, unsigned short port);

    void disConnect();

    bool sendMsg(QByteArray msg, int timeout = -1);
    QByteArray reciMsg(int timeout = -1);

private:

    // 检测读/写事件是否超时
    bool readTimeout(int timeout);
    bool writeTimeout(int timeout);

    // 解决粘包问题
    int readn(char* buf, int count);
    int writen(const char* buf, int count);



// windos环境下的socket类型
#ifdef Q_OS_WIN
    SOCKET m_socket;
#endif
// linux环境下的socket类型
#ifdef Q_OS_LINUX
    int m_socket;
#endif

};

#endif
