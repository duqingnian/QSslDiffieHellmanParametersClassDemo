#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include "SslServer.h"
#include <QSslSocket>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    void startServer(quint16 port);

private slots:
    void onNewConnection();
    void onEncrypted();
    void onReadyRead();

private:
    SslServer *server;
};

#endif // SERVER_H
