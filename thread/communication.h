#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "aescrypto.h"
#include "codec.h"
#include <QObject>
#include <QRunnable>
#include <tcpsocket.h>

class Communication : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Communication(Message& loginRegisterMsg, QObject *parent = nullptr);
    ~Communication();

    inline void stopLoop()
    {
        m_stop = true;
    }

    // 处理数据通讯
    // 发送数据
    void sendMsg(Message *msg);
    // 接收并解析数据
    void parseRecvMessage();

    /** 对具体ResponseCode进行处理*/
    void handleRsaFenfa(QSharedPointer<Message> ptr);
    // 生成秘钥aes
    enum KeyLen{L16 = 16, L24=24, L32=32};    // 秘钥长度
    QByteArray generateAesKey(KeyLen len);

    // 处理用户登录
    void handleLogin();

protected:
    void run() override;

signals:
    void connectFailed();

    void loginOK();
    void registerOK();

    void failedMsg(QByteArray msg);


private:
    TcpSocket *m_socket = nullptr;
    // 用于控制线程何时结束
    bool m_stop = false;

    // 首次通信保存的登录/注册数据
    Message m_loginRegisterMsg;

    // aes秘钥
    QByteArray m_aesKey;
    AesCrypto* m_aes = nullptr;
};

#endif // COMMUNICATION_H
