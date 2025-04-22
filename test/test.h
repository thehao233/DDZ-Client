#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QObject>

class Test : QObject
{
    Q_OBJECT

public:
    Test(QObject* parent = nullptr);
    ~Test();

    void testTcpSocket();

    void testSerialize();

private:

};

#endif
