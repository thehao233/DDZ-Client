#include "datamanager.h"

DataManager *DataManager::getInstance()
{
    static DataManager* manager = nullptr;
    if (manager == nullptr)
    {
        manager = new DataManager();
    }

    return manager;
}

void DataManager::setUserName(QByteArray username)
{
    m_username = username;
}


void DataManager::setPhoneNum(QByteArray phoneNum)
{
    m_phoneNum = phoneNum;
}

void DataManager::setIpAddress(QByteArray ipAddress)
{
    m_ipAddress = ipAddress;
}

void DataManager::setPort(QByteArray port)
{
    m_port = port;
}

void DataManager::setCommunication(Communication *comm)
{
    m_comm = comm;
}

QByteArray DataManager::getUserName()
{
    return m_username;
}

QByteArray DataManager::getPhoneNum()
{
    return m_phoneNum;
}

QByteArray DataManager::getIpAddress()
{
    return m_ipAddress;
}

QByteArray DataManager::getPort()
{
    return m_port;
}

Communication *DataManager::getCommunication()
{
    return m_comm;
}


