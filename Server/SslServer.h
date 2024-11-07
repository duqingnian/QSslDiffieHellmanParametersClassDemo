#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QTcpServer>
#include <QSslSocket>

class SslServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit SslServer(QObject *parent = nullptr) : QTcpServer(parent) {}

protected:
    void incomingConnection(qintptr socketDescriptor) override {
        QSslSocket *sslSocket = new QSslSocket(this);
        if (sslSocket->setSocketDescriptor(socketDescriptor)) {
            addPendingConnection(sslSocket);
        } else {
            delete sslSocket;
        }
    }
};

#endif // SSLSERVER_H
