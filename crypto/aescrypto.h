#ifndef AESCRYPTO_H
#define AESCRYPTO_H

#include <QMap>
#include <QObject>
#include "openssl/evp.h"
#include "openssl/aes.h"

// 对称加密类
class AesCrypto : QObject
{
    Q_OBJECT

public:

    enum AesHashAlgorithm
    {
        // 16位
        AES_ECB_128,
        AES_CBC_128,
        AES_CFB_128,
        AES_OFB_128,
        AES_CTR_128,
        // 24位
        AES_ECB_192,
        AES_CBC_192,
        AES_CFB_192,
        AES_OFB_192,
        AES_CTR_192,
        // 32位
        AES_ECB_256,
        AES_CBC_256,
        AES_CFB_256,
        AES_OFB_256,
        AES_CTR_256,
    };

    // key:秘钥
    AesCrypto(AesHashAlgorithm algorithm, QByteArray key, QObject *parent = nullptr);
    ~AesCrypto();

    QByteArray enCrypto(QByteArray text);
    QByteArray deCrypto(QByteArray text);

    // 加解密类型
    enum CryptoType
    {
        DeCrypto = 0,
        EnCrypto = 1,
    };

private:
    QByteArray crypto(QByteArray text, CryptoType type);

    // 生成对称加密算法使用的初始向量
    void generateiv(unsigned char* iv);

private:
    AesHashAlgorithm m_aesHashAlgorithm;
    // 对称加密的秘钥
    QByteArray m_key;

    using AlgorithmFunc = const EVP_CIPHER *(*)(void);

    // 对称加密算法名和函数之间的映射
    const QMap<AesHashAlgorithm, AlgorithmFunc> m_algorithmMap{
        // 16位
        {AES_ECB_128, EVP_aes_128_ecb},
        {AES_CBC_128, EVP_aes_128_cbc},
        {AES_CFB_128, EVP_aes_128_cfb128},
        {AES_OFB_128, EVP_aes_128_ofb},
        {AES_CTR_128, EVP_aes_128_ctr},
        // 24位
        {AES_ECB_192, EVP_aes_192_ecb},
        {AES_CBC_192, EVP_aes_192_cbc},
        {AES_CFB_192, EVP_aes_192_cfb128},
        {AES_OFB_192, EVP_aes_192_ofb},
        {AES_CTR_192, EVP_aes_192_ctr},
        // 32位
        {AES_ECB_256, EVP_aes_256_ecb},
        {AES_CBC_256, EVP_aes_256_cbc},
        {AES_CFB_256, EVP_aes_256_cfb128},
        {AES_OFB_256, EVP_aes_256_ofb},
        {AES_CTR_256, EVP_aes_256_ctr},
    };
};

#endif
