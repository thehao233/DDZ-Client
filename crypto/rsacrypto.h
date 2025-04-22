#ifndef RSACRYPTO_H
#define RSACRYPTO_H

#include <QCryptographicHash>
#include <QMap>
#include <QObject>
#include "openssl/evp.h"
#include "Base64.h"

class RsaCrypto : public QObject
{
    Q_OBJECT
public:

    enum KeyType{PublicKey, PrivateKey};

    explicit RsaCrypto(QObject *parent = nullptr);
    // 构造并读取磁盘中的秘钥数据
    RsaCrypto(QByteArray fileName, KeyType type, QObject *parent = nullptr);
    ~RsaCrypto();

    // 密钥对生成的位数
    enum KeyLength{
        BITS_1k = 1024,
        BITS_2k = 2048,
        BITS_3k = 3072,
        BITS_4k = 4096,
    };


    // 生成密钥对
    // 指定秘钥对存放的文件位置
    void generatePkey(KeyLength bits, QByteArray pub = "public.pem", QByteArray pri = "private.pem");

    // 从内存中读取秘钥
    void parseStringToKey(QByteArray data, KeyType type);
    void readKey(QByteArray fileName, KeyType type);

    // 使用公钥进行加密
    QByteArray enCrypto(QByteArray data);

    // 使用私钥进行解密
    QByteArray deCrypto(QByteArray data);

    // 指定hash函数，并对计算后的数据进行数字签名
    QByteArray sign(QByteArray data, QCryptographicHash::Algorithm hashAlgo = QCryptographicHash::Sha256);

    // 进行校验
    bool verify(QByteArray sign, QByteArray data, QCryptographicHash::Algorithm hashAlgo = QCryptographicHash::Sha256);

private:
    EVP_PKEY *m_publicKey = NULL;
    EVP_PKEY *m_privateKey = NULL;
    // 用于生成密钥对，由于使用完成时间未定，因此在对象析构时释放
    EVP_PKEY *m_pkey = NULL;

    using AlgorithmFunc = const EVP_MD *(*)(void);
    const QMap<QCryptographicHash::Algorithm, AlgorithmFunc> m_algorithmMap{
        {QCryptographicHash::Md4, EVP_md4},
        {QCryptographicHash::Md5, EVP_md5},
        {QCryptographicHash::Sha1, EVP_sha1},
        {QCryptographicHash::Sha224, EVP_sha224},
        {QCryptographicHash::Sha256, EVP_sha256},
        {QCryptographicHash::Sha384, EVP_sha384},
        {QCryptographicHash::Sha512, EVP_sha512},
        {QCryptographicHash::Sha3_224, EVP_sha3_224},
        {QCryptographicHash::Sha3_256, EVP_sha3_256},
        {QCryptographicHash::Sha3_384, EVP_sha3_384},
        {QCryptographicHash::Sha3_512, EVP_sha3_512},
    };


    // 用于对二进制数据进行Base64编/解码
    Base64 m_base64;
};

#endif // RSACRYPTO_H
