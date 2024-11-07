#include "Server.h"
#include <QSslConfiguration>
#include <QSslKey>
#include <QSslCertificate>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QSslDiffieHellmanParameters>

Server::Server(QObject *parent) : QObject(parent), server(new QTcpServer(this))
{
    connect(server, &QTcpServer::newConnection, this, &Server::onNewConnection);
}

void Server::startServer(quint16 port)
{
    if (server->listen(QHostAddress::Any, port)) {
        qDebug() << "Server started, listening on port" << port;
    } else {
        qDebug() << "Failed to start server:" << server->errorString();
    }
}

void Server::onNewConnection()
{
    QSslSocket *sslSocket = new QSslSocket(this);
    connect(sslSocket, &QSslSocket::encrypted, this, &Server::onEncrypted);
    connect(sslSocket, &QSslSocket::readyRead, this, &Server::onReadyRead);

    if (!server->hasPendingConnections()) {
        delete sslSocket;
        return;
    }

    if (!sslSocket->setSocketDescriptor(server->nextPendingConnection()->socketDescriptor())) {
        qDebug() << "Failed to cast QTcpSocket to QSslSocket.";
        delete sslSocket;
        return;
    }

    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();

    QFile keyFile("D:/project/qt/QDH/demo/server.key");
    if (keyFile.open(QIODevice::ReadOnly)) {
        QSslKey key(&keyFile, QSsl::Rsa);
        sslConfig.setPrivateKey(key);
        keyFile.close();
    } else {
        qDebug() << "Failed to load server private key.";
    }

    QFile certFile("D:/project/qt/QDH/demo/server.crt");
    if (certFile.open(QIODevice::ReadOnly)) {
        QSslCertificate cert(&certFile);
        sslConfig.setLocalCertificate(cert);
        certFile.close();
    } else {
        qDebug() << "Failed to load server certificate.";
    }

    QFile dhFile("D:/project/qt/QDH/demo/dhparam.pem");
    if (dhFile.open(QIODevice::ReadOnly)) {
        QSslDiffieHellmanParameters dhParams = QSslDiffieHellmanParameters::fromEncoded(dhFile.readAll());
        if (dhParams.isValid()) {
            sslConfig.setDiffieHellmanParameters(dhParams);
            qDebug() << "Diffie-Hellman parameters set successfully.";
        } else {
            qWarning() << "Invalid Diffie-Hellman parameters.";
        }
        dhFile.close();
    }

    sslSocket->setSslConfiguration(sslConfig);
    sslSocket->startServerEncryption();
}

void Server::onEncrypted()
{
    qDebug() << "SSL encryption established with client.";
}

void Server::onReadyRead()
{
    QSslSocket *sslSocket = qobject_cast<QSslSocket *>(sender());
    if (!sslSocket) return;

    QByteArray data = sslSocket->readAll();
    QString message = QString::fromUtf8(data);
    qDebug() << "Received from client:" << message;

    QString response = QString("[%1] Server: %2").arg(QDateTime::currentDateTime().toString(), message);
    sslSocket->write(response.toUtf8());
}
