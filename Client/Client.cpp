#include "Client.h"
#include <QDebug>
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QSslKey>
#include <QFile>
#include <QSslDiffieHellmanParameters>

Client::Client(QObject *parent) : QObject(parent), clientSocket(new QSslSocket(this))
{
    connect(clientSocket, &QSslSocket::encrypted, this, &Client::onConnected);
    connect(clientSocket, &QSslSocket::sslErrors, this, &Client::onSslErrors);
    connect(clientSocket, &QSslSocket::readyRead, this, &Client::onReadyRead);
}

void Client::connectToServer(const QString &host, quint16 port)
{
    QSslConfiguration sslConfig = clientSocket->sslConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);

    QFile certFile("D:/project/qt/QDH/demo/server.crt");
    if (certFile.open(QIODevice::ReadOnly)) {
        sslConfig.setCaCertificates(QSslCertificate::fromPath(certFile.fileName()));
        certFile.close();
        qDebug() << "Server CA certificate loaded successfully";
    } else {
        qDebug() << "Failed to load server CA certificate.";
    }

    QFile dhFile("D:/project/qt/QDH/demo/dhparam.pem");
    if (dhFile.open(QIODevice::ReadOnly)) {
        QSslDiffieHellmanParameters dhParams(QSslDiffieHellmanParameters::fromEncoded(dhFile.readAll()));
        if (dhParams.isValid()) {
            sslConfig.setDiffieHellmanParameters(dhParams);
            qDebug() << "Diffie-Hellman parameters loaded successfully from dhparam.pem";
        } else {
            qDebug() << "Failed to load valid Diffie-Hellman parameters from dhparam.pem";
        }
    } else {
        qDebug() << "Could not open dhparam.pem for reading.";
    }

    clientSocket->setSslConfiguration(sslConfig);
    clientSocket->connectToHostEncrypted(host, port);
}

void Client::sendMessage(const QString &message)
{
    if (clientSocket->isEncrypted()) {
        clientSocket->write(message.toUtf8());
    } else {
        qDebug() << "SSL encryption not established. Unable to send message.";
    }
}

void Client::onConnected()
{
    qDebug() << "SSL encryption established with server";
}

void Client::onSslErrors(const QList<QSslError> &errors)
{
    for (const QSslError &error : errors) {
        qDebug() << "SSL Error:" << error.errorString();
    }
    clientSocket->ignoreSslErrors();
}

void Client::onReadyRead()
{
    QByteArray data = clientSocket->readAll();
    QString message = QString::fromUtf8(data);
    qDebug() << "Message from server:" << message;
    emit messageReceived(message);
}
