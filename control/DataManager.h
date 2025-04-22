#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "communication.h"
#include <QByteArray>


// 单例模式的全局数据管理器
class DataManager
{
public:
    static DataManager *getInstance();
    DataManager(const DataManager& other) = delete;
    DataManager& operator=(const DataManager& other) = delete;

    void setUserName(QByteArray username);
    // void setPassword(QByteArray password);
    void setPhoneNum(QByteArray phoneNum);
    void setIpAddress(QByteArray ipAddress);
    void setPort(QByteArray port);
    void setCommunication(Communication *comm);

    QByteArray getUserName();
    // QByteArray getPassword();
    QByteArray getPhoneNum();
    QByteArray getIpAddress();
    QByteArray getPort();
    Communication* getCommunication();

private:
    DataManager() = default;
    ~DataManager() = default;

private:
    QByteArray m_username;
    // QByteArray m_password;
    QByteArray m_phoneNum;
    QByteArray m_ipAddress;
    QByteArray m_port;

    Communication *m_comm;
};

#endif // DATAMANAGER_H
