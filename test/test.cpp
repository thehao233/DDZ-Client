#include "test.h"
#include "codec.h"
#include <qdebug.h>

Test::Test(QObject *parent) : QObject(parent)
{

}

Test::~Test()
{

}

void Test::testSerialize()
{
    Codec codec;
    Message msg;
    msg.username = "aaa";
    msg.data1 = "sss";
    msg.data2 = "ddd";
    msg.data3 = "fff";
    msg.reqCode = RequestCode::UserLogin;
    msg.resCode = ResponseCode::LoginOK;
    QByteArray encodemsg = codec.encodeMsg(&msg);

    qDebug() << "编码后的数据" << encodemsg;

    QSharedPointer<Message> decodemsg = codec.decodeMsg(encodemsg);
    qDebug() << "解码后的数据" << decodemsg->username;
}
