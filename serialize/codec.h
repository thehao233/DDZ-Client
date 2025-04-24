#ifndef CODEC_H
#define CODEC_H

#include "Information.pb.h"

#include <QByteArray>
#include <QSharedPointer>

struct Message
{
    QByteArray username;
    QByteArray roomname;
    QByteArray data1;
    QByteArray data2;
    QByteArray data3;
    RequestCode reqCode;
    ResponseCode resCode;
};

class Codec
{
public:
    Codec() = default;
    Codec(QByteArray codedMsg);
    Codec(Message* uncodedMsg);

    // 数据编解码
    QByteArray encodeMsg(Message* uncodedMsg);
    QSharedPointer<Message> decodeMsg(QByteArray codedMsg);

    // 重新装载数据
    void reload(QByteArray codedMsg);
    void reload(Message* uncodedMsg);

private:
    QByteArray m_codedMsg;
    // 用于处理数据的编解码
    Information m_inf;
};

#endif // CODEC_H
