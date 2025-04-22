#ifndef BASE64_H
#define BASE64_H

#include <QObject>

class Base64 : QObject
{
public:
    Base64(QObject *parent = nullptr);

    // 编码
    QByteArray enCode(QByteArray data);
    QByteArray enCode(const char* data, int length);


    // 解码
    QByteArray deCode(QByteArray data);
    QByteArray deCode(const char* data, int length);
};

#endif // BASE64_H
