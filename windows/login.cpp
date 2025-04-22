#include "gamepanel.h"
#include "login.h"
#include "ui_login.h"
#include "communication.h"
#include "datamanager.h"
#include "gamemode.h"

#include <QCryptographicHash>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QThreadPool>
#include <QMessageBox>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>


login::login(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentWidget(ui->loginPage);

    connect(ui->homePageBtn, &QPushButton::clicked, this, [=]{
        ui->stackedWidget->setCurrentWidget(ui->loginPage);
    });

    connect(ui->netSetBtn, &QPushButton::clicked, this, [=]{
        ui->stackedWidget->setCurrentWidget(ui->netSetPage);
    });

    connect(ui->regUserBtn, &QPushButton::clicked, this, [=]{
        ui->stackedWidget->setCurrentWidget(ui->regPage);
    });


    QRegularExpression express("^[a-zA-Z\\d_]{3,16}");
    QRegularExpressionValidator *userNameValidator = new QRegularExpressionValidator(express, this);
    ui->loginUserNameEdit->setValidator(userNameValidator);
    ui->regUserNameEdit->setValidator(userNameValidator);

    // 链接登录、注册的功能和按钮
    connect(ui->loginBtn, &QPushButton::clicked, this, [=]{
        onLogin();
    });

    connect(ui->regBtn, &QPushButton::clicked, this, [=]{
        onRegister();
    });

    ui->ipAddressEdit->setText("47.95.43.135");
    ui->portEdit->setText("10000");

    // ui->loginUserNameEdit->setText("aaa");
    // ui->loginPwdEdit->setText("000");

    // ui->regUserNameEdit->setText("aaa");
    // ui->regPwdEdit->setText("000");
    // ui->regPhoneNumEdit->setText("123456");

    // 设置最大子线程个数
    QThreadPool::globalInstance()->setMaxThreadCount(8);

    // 加载保存在本地的用户数据
    loadUserInfo();
}

login::~login()
{
    delete ui;
}

bool login::verifyData(QLineEdit* edit)
{
    if (edit->hasAcceptableInput() == false)
    {
        edit->setStyleSheet("border: 2px solid red;");
        return false;
    }
    else
    {
        edit->setStyleSheet("none");
    }
    return true;
}

void login::startConnect(Message *msg)
{
    if (!m_isConnected)
    {
        // 没有连接 则建立连接
        /**
         * 对于首次通信，需要先建立连接(完成aes分发)，才能发送具体数据
         * 因此需要将 登录/注册 数据先暂存起来，然后 在完成aes分发后 再进行数据发送
         *
         * 下方else不能移除，原因：
         * 1. 直接移除会导致 DataManager::getInstance()->getCommunication()->sendMsg(msg); 的调用可能先于aes分发，导致错误的数据发送，因为线程的并行执行
         * 2. 若想直接移除，可能需要添加 信号等方法来使线程同步
         */
        qDebug() << "parseRecvMessage， 用户名：" << msg->username
                 << " 是否为登录请求" << (msg->reqCode == RequestCode::UserLogin);
        Communication *comm = new Communication(*msg);

        qDebug() << "startConn, username:" << msg->username;

        // 链接 连接失败 的信号和槽函数
        connect(comm, &Communication::connectFailed, this, [=]{
            QMessageBox::critical(this, "连接服务器", "连接服务器失败...");
            m_isConnected = false;
            // 移出全局的单例数据中的通信对象 并释放资源
            // Communication *comm = DataManager::getInstance()->getCommunication();
            // delete comm;
            DataManager::getInstance()->setCommunication(nullptr);
        });

        // 链接 登录成功 的信号和槽函数
        connect(comm, &Communication::loginOK, this, [=]{
            // 保存用户信息
            saveUserInfo();
            // 将用户名保存到单例对象
            DataManager::getInstance()->setUserName(ui->loginUserNameEdit->text().toUtf8());
            // 跳转到模式选择界面
            qDebug() << "开始跳转到模式选择界面....";
            GameMode* mode = new GameMode;
            mode->show();
            // 隐藏用户登录界面
            accept();
        });

        // 链接 注册成功 的信号和槽函数
        connect(comm, &Communication::registerOK, this, [=]{
            // 跳转到登录界面
            ui->stackedWidget->setCurrentWidget(ui->loginPage);
        });

        // 链接 出现错误 的信号和槽函数
        connect(comm, &Communication::failedMsg, this, [=](QByteArray msg){
            // 弹出错误窗口，显示错误信息
            QMessageBox::critical(this, "错误信息", msg);
        });

        m_isConnected = true;
        // 将通信对象存放到全局的单例数据中
        DataManager::getInstance()->setCommunication(comm);
        QThreadPool::globalInstance()->start(comm);
    }
    else
    {
        // 和服务器进行通信
        // 向服务器发送数据
        DataManager::getInstance()->getCommunication()->sendMsg(msg);
    }
}

void login::onLogin()
{
    bool flag = verifyData(ui->loginUserNameEdit);
    if (flag)
    {
        // 设置全局数据
        updateIpPort();

        // 设置消息体
        Message msg;
        msg.username = ui->loginUserNameEdit->text().toUtf8();
        msg.reqCode = RequestCode::UserLogin;
        // 对密码进行加密
        QByteArray pwd = ui->loginPwdEdit->text().toUtf8();
        QCryptographicHash hs(QCryptographicHash::Sha224);
        hs.addData(pwd);
        msg.data1 = hs.result().toHex();

        qDebug() << "onLogin, username:" << msg.username;
        // 连接服务器
        startConnect(&msg);
    }
}

void login::onRegister()
{
    bool flag = verifyData(ui->regUserNameEdit);
    if (flag)
    {
        // 设置全局数据
        updateIpPort();

        // 设置消息体
        Message msg;
        msg.username = ui->regUserNameEdit->text().toUtf8();
        msg.data2 = ui->regPhoneNumEdit->text().toUtf8();
        msg.reqCode = RequestCode::Register;
        // 对密码进行加密
        QByteArray pwd = ui->regPwdEdit->text().toUtf8();
        QCryptographicHash hs(QCryptographicHash::Sha224);
        hs.addData(pwd);
        msg.data1 = hs.result().toHex();

        // 连接服务器
        startConnect(&msg);
    }
}

void login::updateIpPort()
{
    DataManager *data = DataManager::getInstance();
    data->setIpAddress(ui->ipAddressEdit->text().toUtf8());
    data->setPort(ui->portEdit->text().toUtf8());
}

void login::saveUserInfo()
{
    if (ui->saveUserInfoCheckBox->isChecked())
    {
        // 将用户数据写入到本地磁盘

        // 设置json对象
        QJsonObject obj;
        obj.insert("username",  ui->loginUserNameEdit->text());
        // obj.insert("password",  ui->loginPwdEdit->text());

        // 对数据进行加密
        AesCrypto aes(AesCrypto::AES_CBC_128, m_localAesKey.left(16));
        QByteArray cryptoedPwd = aes.enCrypto(ui->loginPwdEdit->text().toUtf8());
        obj.insert("password",  cryptoedPwd.toHex().data());

        // 将josn对象转换为字符串
        QJsonDocument doc(obj);
        QByteArray data = doc.toJson();

        // 将数据写入磁盘
        QFile file("userInfo.json");
        file.open(QFile::WriteOnly);
        file.write(data);
        file.close();

    }
    else
    {
        // 将本地磁盘存储的用户数据删除
        QFile file("userInfo.json");
        file.remove();
    }
}

void login::loadUserInfo()
{
    QFile file("userInfo.json");
    bool flag  = file.open(QFile::ReadOnly);

    if (flag)
    {
        ui->saveUserInfoCheckBox->setChecked(true);
        QByteArray data = file.readAll();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();

        QString username = obj.value("username").toString();
        QString cryptoedPwd = obj.value("password").toString();

        // 对数据进行解密
        AesCrypto aes(AesCrypto::AES_CBC_128, m_localAesKey.left(16));
        QByteArray password = aes.deCrypto(QByteArray::fromHex(cryptoedPwd.toUtf8()));

        ui->loginUserNameEdit->setText(username);
        ui->loginPwdEdit->setText(password.data());
    }
    else
    {
        ui->saveUserInfoCheckBox->setChecked(false);
    }
}
