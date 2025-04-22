#include "aescrypto.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>

AesCrypto::AesCrypto(AesHashAlgorithm algorithm, QByteArray key, QObject *parent) : QObject(parent)
{
    switch(algorithm)
    {
    case AES_CBC_128:
    case AES_ECB_128:
    case AES_OFB_128:
    case AES_CFB_128:
    case AES_CTR_128:
        assert(key.size() == 16);
        break;
    case AES_CBC_192:
    case AES_ECB_192:
    case AES_OFB_192:
    case AES_CFB_192:
    case AES_CTR_192:
        assert(key.size() == 24);
        break;
    case AES_CBC_256:
    case AES_ECB_256:
    case AES_OFB_256:
    case AES_CFB_256:
    case AES_CTR_256:
        assert(key.size() == 32);
        break;
    }

    m_aesHashAlgorithm = algorithm;
    m_key = key;
}

AesCrypto::~AesCrypto()
{

}

QByteArray AesCrypto::enCrypto(QByteArray text)
{
    return crypto(text, EnCrypto);
}

QByteArray AesCrypto::deCrypto(QByteArray text)
{
    return crypto(text, DeCrypto);
}

QByteArray AesCrypto::crypto(QByteArray text, CryptoType type)
{

    // 生成上下文信息
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    assert(ctx);
    // 初始化哈希函数的上下文信息
    // ENGINE *impl设置为null，使用openssl默认提供的引擎
    // iv : 对称加密算法使用的初始向量，长度为16位，进行加/解密使用的vi应相同
    // enc : 0-解密，1-加密
    unsigned char iv[AES_BLOCK_SIZE];
    assert(iv);
    generateiv(iv);
    int ret = EVP_CipherInit_ex(ctx, m_algorithmMap.value(m_aesHashAlgorithm)(),
                            NULL, reinterpret_cast<unsigned char*>(m_key.data()), iv, type);
    assert(ret);

    // 加解密后数据的大致长度
    int length = text.size() + 1;
    if (text.size() % AES_BLOCK_SIZE)
    {
        length = (length / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    }

    unsigned char *out = new unsigned char[length];
    // 写入数据的长度 和 总数据长度
    int outLen = 0, totalLen = 0;
    // 对数据进行加/解密, 并取出 除最后一组外的数据
    ret = EVP_CipherUpdate(ctx, out, &outLen,
                        reinterpret_cast<const unsigned char *>(text.data()), text.size());
    assert(ret);
    totalLen += outLen;

    // 取出最后一组数据
    ret = EVP_CipherFinal_ex(ctx, out+outLen, &outLen);
    assert(ret);
    totalLen += outLen;

    QByteArray cryptoText(reinterpret_cast<char *>(out), totalLen);

    // 释放资源
    delete[] out;
    EVP_CIPHER_CTX_free(ctx);

    return cryptoText;
}

void AesCrypto::generateiv(unsigned char *iv)
{
    // 此处先计算哈希
    QCryptographicHash hs(QCryptographicHash::Md5);
    hs.addData(m_key);
    QByteArray res = hs.result();
    // 将经过hash计算的对称加密的秘钥的前AES_BLOCK_SIZE位作为初始向量
    for (int i = 0; i < AES_BLOCK_SIZE; ++i) {
        iv[i] = res.at(i);
    }
}
