#include "codec.h"
#include <qdebug.h>
#include <string>

Codec::Codec(QByteArray codedMsg)
{
    reload(codedMsg);
}

Codec::Codec(Message *uncodedMsg)
{
    reload(uncodedMsg);
}

QByteArray Codec::encodeMsg(Message *uncodedMsg)
{
    // 设置编解码器数据
    m_inf.set_username(uncodedMsg->username.toStdString());
    m_inf.set_roomname(uncodedMsg->roomname.toStdString());
    m_inf.set_data1(uncodedMsg->data1.toStdString());
    m_inf.set_data2(uncodedMsg->data2.toStdString());
    m_inf.set_data3(uncodedMsg->data3.toStdString());
    m_inf.set_reqcode(uncodedMsg->reqCode);
    m_inf.set_rescode(uncodedMsg->resCode);
    std::string out;
    m_inf.SerializeToString(&out);
    // 设置编码后的数据
    m_codedMsg = QByteArray::fromStdString(out);
    return m_codedMsg;
}

QSharedPointer<Message> Codec::decodeMsg(QByteArray codedMsg)
{
    // 设置编码后的数据
    m_codedMsg = codedMsg;
    // 设置编解码器数据
    m_inf.ParseFromString(codedMsg.toStdString());
    // 设置编码后的数据
    QSharedPointer<Message> uncodeMsg(new Message);
    uncodeMsg->username = QByteArray::fromStdString(m_inf.username());
    uncodeMsg->roomname = QByteArray::fromStdString(m_inf.roomname());
    uncodeMsg->data1 = QByteArray::fromStdString(m_inf.data1());
    uncodeMsg->data2 = QByteArray::fromStdString(m_inf.data2());
    uncodeMsg->data3 = QByteArray::fromStdString(m_inf.data3());
    uncodeMsg->reqCode = m_inf.reqcode();
    uncodeMsg->resCode = m_inf.rescode();
    return uncodeMsg;
}

void Codec::reload(QByteArray codedMsg)
{
    decodeMsg(codedMsg);
}

void Codec::reload(Message *uncodedMsg)
{
    encodeMsg(uncodedMsg);
}
