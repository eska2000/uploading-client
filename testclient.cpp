#include "testclient.h"

TestClient::TestClient(QObject *parent) : QObject(parent)
{
    m_file = new QFile("1.mp4");
//    m_file = new QFile("/Users/eska2000/Downloads/ras.img");

    start();
}

TestClient::TestClient(QFile &file, const QString &server, const int port, QObject *parent) : QObject(parent), m_file(&file), serverAddress(server), serverPort(port)
{
    start();
}

void TestClient::start()
{
    if (!m_file->open(QIODevice::ReadOnly)) {
        qWarning()<<"Wrong file address";
        emit error("Wrong file address");
        return;
    }

    qDebug()<<"Connecting...";

    socket.connectToHost(serverAddress, serverPort);

    QFileInfo fileInfo(*m_file);
    fileName = fileInfo.fileName();
    fileSize = fileInfo.size();

    qDebug()<<"File: "<<fileName<<" : "<<fileInfo.size();

    connect(&socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(error(QAbstractSocket::SocketError)));
    connect(&socket,SIGNAL(connected()),this,SLOT(connected()));
    connect(&socket,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(&socket,SIGNAL(readyRead()),this,SLOT(readyRead()));
    connect(&socket,SIGNAL(bytesWritten(qint64)),this,SLOT(bytesWritten(qint64)));
}

void TestClient::error(QAbstractSocket::SocketError socketError)
{
    if (socketError == QTcpSocket::RemoteHostClosedError)
        return;

    qWarning()<<"Something wrong!";
    qWarning()<<socket.errorString();
    socket.close();
    m_file->close();

    emit error(socket.errorString());
}

void TestClient::connected()
{
    qDebug()<<"Connected to host";
    socket.write("file");
}

void TestClient::readyRead()
{
    QByteArray data = socket.readAll();
    qDebug()<<"Recieved: "<<data;
    if (QString(data) == "#server/file/name") {
        socket.write(fileName.toUtf8());
    }
    else if (QString(data) == "#server/file/size") {
        socket.write(QString::number(fileSize).toUtf8());
    }
    else if (QString(data) == "#server/file/data") {
        qDebug()<<"Sending file...";
        QByteArray bytes = m_file->readLine(64*1024);
        while(!bytes.isNull()) {
            socket.write(bytes);
            if (socket.bytesToWrite()>1024*1024*50)
                socket.flush(); // Если считано более 50 МБ, выгружаем, чтобы не перегружать оперативную память
            bytes = m_file->readLine(64*1024);
        }
        qDebug()<<"File sent";
        emit fileHasBeenUploaded();
    }
}

void TestClient::bytesWritten(qint64 bytes)
{
    totalSent+=bytes;
    emit bytesSent(totalSent);
    emit percentSent((double)totalSent/fileSize*100);
}
