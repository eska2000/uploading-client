#ifndef TESTCLIENT_H
#define TESTCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QtDebug>
#include <QFile>
#include <QFileInfo>

class TestClient : public QObject
{
    Q_OBJECT
public:
    explicit TestClient(QObject *parent = 0);
    explicit TestClient(QFile &file, const QString &server, const int port, QObject *parent = 0);

private:
    QFile *m_file;
    QString serverAddress = "127.0.0.1";
    int serverPort = 33333;
    QTcpSocket socket;
    qint64 fileSize, totalSent = 0;
    QString fileName;

    void start();

private slots:
    void error(QAbstractSocket::SocketError);
    void connected();
    void bytesWritten(qint64 bytes);
    void readyRead();

signals:
    void fileHasBeenUploaded();
    void error(QString error);
    void bytesSent(qint64 bytes);
    void percentSent(int percent);

public slots:

};

#endif // TESTCLIENT_H
