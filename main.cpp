#include <QApplication>
#include <QResource>
#include <qdebug.h>

#include "login.h"
#include "cards.h"
#include "aescrypto.h"
#include "codec.h"
#include "rsacrypto.h"

void testSerialize()
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

void testAesCrypto()
{
    AesCrypto aes(AesCrypto::AesHashAlgorithm::AES_CBC_128, "1234567887654321");
    QByteArray text("我们在调用MySQL API的使用需要加载的动态库为libmysql.dll，它对应的导入库为libmysql.lib，在该窗口的附加依赖项位置指定的就是这个导入库的名字。编译编写好的项目之后，在对应的项目目录中会生成一个可执行程序，打开这个目录，将上面步骤中下载的用于MySQL数据库加密的动态库拷贝到该目录中，这样程序就可以正常执行了，否则会提示无法加载某个动态库。");
    QByteArray enCrypyoText = aes.enCrypto(text);
    qDebug() << "加密数据：" << enCrypyoText;

    QByteArray deCryptoText = aes.deCrypto(enCrypyoText);
    qDebug() << "解密数据：" << deCryptoText.data();
}

void testRsaCrypto()
{
    RsaCrypto rsa;
    rsa.generatePkey(RsaCrypto::BITS_2k);
    qDebug() << "正在生成密钥对...";

    QByteArray text("我们在调用MySQL");

    RsaCrypto rsa1("public.pem", RsaCrypto::PublicKey);
    QByteArray enCrypyoText = rsa1.enCrypto(text);
    qDebug() << "加密数据：" << enCrypyoText;

    RsaCrypto rsa2("private.pem", RsaCrypto::PrivateKey);
    QByteArray deCryptoText = rsa2.deCrypto(enCrypyoText);
    qDebug() << "解密数据：" << deCryptoText.data();

    QByteArray signedData = rsa2.sign(deCryptoText);
    bool flag = rsa.verify(signedData, text);
    qDebug() << "校验结果：" << flag;

}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<Cards>("Cards&");
    qRegisterMetaType<Cards>("Cards");
    //QResource::registerResource("./resource.rcc");

    // testAesCrypto();
    // testRsaCrypto();
    // testSerialize();

    login w;
    int ret = w.exec();
    if (ret == 1)
    {
        return a.exec();
    }



}
