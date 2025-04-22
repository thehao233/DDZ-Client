#include "tcpsocket.h"
#include <qdebug.h>

TcpSocket::TcpSocket(QObject *parent) : QObject(parent)
{
    int ret = 0;
// windows 环境使用socket需要先初始化相关的库文件
#ifdef Q_OS_WIN
    WSADATA data;
    ret = WSAStartup(MAKEWORD(2, 2), &data);
#endif
    if (ret != 0)
    {
        qDebug("windows环境初始化库文件失败");
    }
}

TcpSocket::TcpSocket(QByteArray ipAddress, unsigned short port, QObject *parent) : TcpSocket(parent)
{
    connectToHost(ipAddress, port);
}

TcpSocket::~TcpSocket()
{
    // windows平台需要对加载的资源进行释放
#ifdef Q_OS_WIN
    WSACleanup();
#endif
}

bool TcpSocket::connectToHost(QByteArray ipAddress, unsigned short port)
{
    // qDebug() << "before ip:" << ipAddress << " port:" << port;

    // AF_INET:建立ipv4链接
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    assert(m_socket);

    struct sockaddr_in saddr_in;
    memset(&saddr_in, 0, sizeof(saddr_in));
    saddr_in.sin_family = AF_INET;
    // 端口号需设置为大端存储
    saddr_in.sin_port = htons(port);
    saddr_in.sin_addr.s_addr = inet_addr(ipAddress.data());

    // 使用与QOBJECT提供函数的同名其他函数时需要加上作用域符号
    int ret = ::connect(m_socket, reinterpret_cast<struct sockaddr*>(&saddr_in), sizeof(saddr_in));

    // qDebug() << "链接结果：" << ret;
    // if (ret == SOCKET_ERROR) {
    //     int error = WSAGetLastError();
    //     qDebug() << "连接失败，错误码: " << error;
    // }
    bool flag = ret == 0 ? true : false;
    return flag;
}

void TcpSocket::disConnect()
{
    if (m_socket != NULL)
    {
#ifdef Q_OS_WIN
        closesocket(m_socket);
#endif

#ifdef Q_OS_LINUX
        close(m_socket);
#endif
    }

}

bool TcpSocket::sendMsg(QByteArray msg, int timeout)
{
    bool flag = writeTimeout(timeout);

    if(flag == false)
    {
        perror("sendMsg 超时");
        return false;
    }

    int headLen = htonl(msg.size());
    int totalLen = msg.size() + sizeof(int);

    // 申请一块新内存，用于存放发送的数据
    char* sendbuf = new char[totalLen];
    assert(sendbuf);
    memcpy(sendbuf, &headLen, sizeof(int));
    memcpy(sendbuf + sizeof(int), msg.data(), msg.size());
    int ret = writen(sendbuf, totalLen);

    qDebug() << "应发送数据：" << totalLen << " 实际发送数据：" << ret
             << "是否正确发送：" << (ret == totalLen);

    delete[] sendbuf;
    return (ret == totalLen);
}

QByteArray TcpSocket::reciMsg(int timeout)
{
    bool flag = readTimeout(timeout);
    if(flag == false)
    {
        perror("reciMsg timeout");
        return QByteArray();
    }

    // 接收数据的长度
    int headLen = 0;
    // readn 之前实现有问题，现在正常了
    int ret = readn((char*)&headLen, sizeof(int));
    if (ret == 0)
    {
        perror("reciMsg length error");
        return QByteArray();
    }
    int dataLen = ntohl(headLen);
    qDebug() << "应接收的数据长度:" << dataLen;

    // 申请一块新内存，用于存放接收的数据
    char* recibuf = new char[dataLen];
    assert(recibuf);
    // memset(recibuf, 0, dataLen);
    ret = readn(recibuf, dataLen);
    qDebug() << "实际接收的数据长度:" << ret;

    QByteArray msg;
    if (ret == dataLen)
    {
        msg = QByteArray(recibuf, dataLen);
    }
    else
    {
        msg = QByteArray();
    }

    delete[] recibuf;
    return msg;
}

bool TcpSocket::readTimeout(int timeout)
{
    if (timeout == -1)
    {
        // 阻塞等待读取数据
        return true;
    }

    // nfds的含义：
    // windows：无具体含义，设置为0 即可
    // linux: 待监听文件描述符的最大数量
#ifdef Q_OS_WIN
    int nfds = 0;
#endif
#ifdef Q_OS_LINUX
    int nfds = m_socket + 1;
#endif

    fd_set rdset;
    FD_ZERO(&rdset);
    FD_SET(m_socket, &rdset);

    // 设置监听时长
    struct timeval time;
    time.tv_sec = timeout;
    time.tv_usec = 0;
    // 设置监听读事件
    int ret = select(nfds, &rdset, NULL, NULL, &time);
    bool flag = ret > 0 ? true : false;
    return flag;
}

bool TcpSocket::writeTimeout(int timeout)
{
    if (timeout == -1)
    {
        // 阻塞等待写入数据
        return true;
    }

    // nfds的含义：
    // windows：无具体含义，设置为0 即可
    // linux: 待监听文件描述符的最大数量
#ifdef Q_OS_WIN
    int nfds = 0;
#endif
#ifdef Q_OS_Linux
    int nfds = m_socket + 1;
#endif

    fd_set wset;
    FD_ZERO(&wset);
    FD_SET(m_socket, &wset);

    // 设置监听时长
    struct timeval time;
    time.tv_sec = timeout;
    time.tv_usec = 0;
    // 设置监听写事件
    int ret = select(nfds, NULL, &wset, NULL, &time);
    bool flag = ret > 0 ? true : false;
    return flag;
}

int TcpSocket::readn(char* buf, int count)
{
    assert(m_socket);
    int last = count; // 剩余读出的字节数
    int size = 0; // 每次读出的字节数
    char* ptr = buf; // 读数据的起始地址

    while (last > 0)
    {
        size = recv(m_socket, ptr, last, 0);
        if (size < 0)
        {
            perror("未读出数据，服务器断开连接");
            return -1;
        }else if (size == 0)
        {
            // 数据已接收完毕
            break;
        }
        last -= size;
        ptr += size;
    }
    // 实际接收 = 应接收 - 剩余未接收
    return count - last;
}

int TcpSocket::writen(const char* buf, int count)
{
    qDebug() << "开始发送数据....";

    assert(m_socket);
    int last = count; // 剩余写入的字节数
    const char* ptr = buf; // 写数据的起始地址
    int size = 0; // 每次写入的字节数

    while (last > 0)
    {
        size = send(m_socket, ptr, last, 0);
        if (size < 0)
        {
            perror("写入数据出错。。。");
            return -1;
        }
        else if (size == 0)
        {
            // 未写入数据
            continue;
        }
        last -= size;
        ptr += size;
    }

    return count - last;
}
