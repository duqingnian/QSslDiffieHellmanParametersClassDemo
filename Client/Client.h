#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QSslSocket>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    void connectToServer(const QString &host, quint16 port);
    void sendMessage(const QString &message);

signals:
    void messageReceived(const QString &message);

private slots:
    void onConnected();
    void onSslErrors(const QList<QSslError> &errors);
    void onReadyRead();

private:
    QSslSocket *clientSocket;
};

#endif // CLIENT_H
