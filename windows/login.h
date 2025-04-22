#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QLineEdit>
#include <codec.h>

namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

private:
    bool verifyData(QLineEdit* edit);

    // 连接服务器
    // msg存储 连接/发送的数据
    void startConnect(Message *msg);

    // 处理登录事件
    void onLogin();
    // 处理注册事件
    void onRegister();

    // 更新全局数据中的ip和port
    void updateIpPort();
    // 更新全局数据中的username和password
    // void updateLoginUserInfo();
    // void updateRegUserInfo();

    // 记住用户数据 和 加载用户数据
    void saveUserInfo();
    void loadUserInfo();

private:
    Ui::login *ui;

    // 是否与服务器建立了连接
    bool m_isConnected = false;

    // 解密保存到本地用户信息的AesKey
    QByteArray m_localAesKey = "道阻且长,行则将至...";
};

#endif // LOGIN_H
