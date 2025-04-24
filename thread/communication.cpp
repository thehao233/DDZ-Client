#include "communication.h"

#include <DataManager.h>
#include <QDateTime>
#include <QMessageBox>
#include <QThread>
#include <aescrypto.h>
#include <qdebug.h>
#include <rsacrypto.h>

Communication::Communication(Message& loginRegisterMsg, QObject *parent)
    : QObject{parent}, QRunnable()
{
    m_loginRegisterMsg = loginRegisterMsg;
    qDebug() << "Communication constructer, username:" << m_loginRegisterMsg.username;
    // 设置为自动析构
    setAutoDelete(true);
}

Communication::~Communication()
{
    // 释放资源
    if (m_socket != nullptr)
    {
        delete m_socket;
    }

    if (m_aes != nullptr)
    {
        delete m_aes;
    }
}

void Communication::run()
{

    m_socket = new TcpSocket;
    // 连接服务器
    DataManager* obj = DataManager::getInstance();
    bool flag = m_socket->connectToHost(obj->getIpAddress(), obj->getPort().toInt());
    qDebug() << "========== 通信的子线程id: " << QThread::currentThread() << "connected ? " << flag;
    if(!flag)
    {
        // 通知主线程连接诶失败了
        emit connectFailed();
        delete m_socket;
        return;
    }

    while (!m_stop)
    {
        // 首先接收服务器发来的非对称加密的公钥和公钥的数字签名
        // 生成对称加密的秘钥，使用该秘钥对公钥进行加密，并将其发送给服务器
        // 发送登录/注册的请求
        parseRecvMessage();
    }

    // 断开连接
    m_socket->disConnect();
    delete m_socket;
}

void Communication::sendMsg(Message *msg)
{

    qDebug() << "开始进行数据序列化...";
    // 对数据进行序列化
    Codec code;
    QByteArray data = code.encodeMsg(msg);
    qDebug() << "数据序列化完成...";

    // 如果已完成aes分发，则对数据进行加密
    if (m_aes != nullptr)
    {
        qDebug() << "开始使用aes对数据进行加密...";
        data = m_aes->enCrypto(data);
        qDebug() << "aes加密完成...";
    }

    // 进行base64编码

    // 发送数据
    bool flag = m_socket->sendMsg(data);
    if (flag)
        qDebug() << "发送数据完成...";
}

void Communication::parseRecvMessage()
{
    // 设置阻塞时长为2秒
    QByteArray msg = m_socket->reciMsg(2);
    if(msg.isEmpty()) return;

    // 对数据进行base64解码

    // 对数据进行解密

    // 对序列化数据进行解码
    Codec code;
    QSharedPointer<Message> msgPtr = code.decodeMsg(msg);
    qDebug() << "ResCode: " << (int)msgPtr->resCode << ", "
             << msgPtr->data1.data() << ", " << msgPtr->data2.data();


    // 根据接收的数据来进行具体的处理
    switch (msgPtr->resCode) {
    case ResponseCode::RsaFenfa:
        /** 处理rsa密钥分发操作 */
        // 生成对称加密秘钥
        // 使用对称加密秘钥 加密 数字签名的公钥
        // 将加密后的数据发送给服务器
        handleRsaFenfa(msgPtr);
        break;
    case ResponseCode::AesVerifyOK:
        qDebug() << "Aes校验成功";
        m_aes = new AesCrypto(AesCrypto::AesHashAlgorithm::AES_CBC_256, m_aesKey);
        assert(m_aes);
        // 将存储的 login/register 数据发送出去
        qDebug() << "parseRecvMessage， 用户名：" << m_loginRegisterMsg.username
                 << " 是否为登录请求" << (m_loginRegisterMsg.reqCode == RequestCode::UserLogin);
        sendMsg(&m_loginRegisterMsg);
        // m_stop = true;
        break;

    case ResponseCode::LoginOK:
        // 发射信号， 跳转到模式选择界面
        emit loginOK();
        qDebug() << "用户登录成功...";
        break;
    case ResponseCode::RegisterOK:
        // 发射信号， 跳转到登录界面
        emit registerOK();
        qDebug() << "用户注册成功...";
        break;
    case ResponseCode::Failed:
        // 发射信号， 弹出错误窗口
        emit failedMsg(msgPtr->data1);
        qDebug() << "出现了错误... " << msgPtr->data1.data();
        break;
    default:
        break;
    }

}

void Communication::handleRsaFenfa(QSharedPointer<Message> msgPtr)
{
    /** 处理密钥分发操作 */
    qDebug() << "开始处理密钥分发操作...";

    // 首先对服务器发送的数字签名进行校验
    RsaCrypto rsa;
    QByteArray pubKey = msgPtr->data1;
    QByteArray signData = msgPtr->data2;
    rsa.parseStringToKey(pubKey, RsaCrypto::KeyType::PublicKey);
    bool flag =rsa.verify(signData, pubKey);
    if (!flag)
    {
        qDebug() << "rsa校验失败...";
        return;
    }

    qDebug() << "rsa校验成功，生成aes秘钥...";

    // 生成对称加密秘钥
    m_aesKey = generateAesKey(KeyLen::L32);

    // 使用公钥加密 对称加密秘钥
    QByteArray enCryptoData = rsa.enCrypto(m_aesKey);

    // 将加密后的数据发送给服务器
    Message msg;
    msg.reqCode = RequestCode::AesFenfa;
    msg.data1 = enCryptoData;
    // 计算aesKey的哈希值，用于校验
    QCryptographicHash hs(QCryptographicHash::Algorithm::Sha256);
    hs.addData(m_aesKey);
    msg.data2 = hs.result();

    sendMsg(&msg);
}

QByteArray Communication::generateAesKey(KeyLen len)
{
    QByteArray time = QDateTime::currentDateTime().toString("yyyy.MM.dd-hh:mm:ss.zzz").toUtf8();
    // 进行哈希计算
    QCryptographicHash hash(QCryptographicHash::Sha3_384);
    hash.addData(time);
    time = hash.result();
    time = time.left(len);
    return time;
}

void Communication::handleLogin()
{
    /** 处理登录操纵 */

}


