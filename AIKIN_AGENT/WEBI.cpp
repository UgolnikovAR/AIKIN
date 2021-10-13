#include "WEBI.h"
#include <QDebug>
#include <QByteArray>
#include <QDataStream>
#include <QTime>
#include <QTimer>
#include <QFile>

#include <QHostAddress>

WEBI::WEBI()
    :QThread(nullptr)
{
    socket = new QTcpSocket();

    connect(socket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(socket, SIGNAL(readyRead()), SLOT(slotReadyRead()));

    connect(this, SIGNAL(signalSentToServer(QString)), SLOT(slotSentToServer(QString)));
    connectToServer("localhost", 2323);


    connect(this, SIGNAL(sigTest()), SLOT(slotTest()), Qt::DirectConnection);


}


void WEBI::run()
{
    emit signalSentToServer(QString("Signal Call from run()"));
    emit sigTest();

    //connectToServer("localhost", 2323);

    using namespace std;
    cout << "WEBI in work" << endl;

    at_work();
}


void WEBI::at_work()
{
    using namespace std;

    if(socket->state() == QAbstractSocket::ConnectedState)
        cout << "connected!" << endl;
    else if(socket->state() == QAbstractSocket::ConnectingState)
        cout << "connecting..." << endl;
    else if(socket->state() == QAbstractSocket::UnconnectedState)
        cout << "unconnected" << endl;
    else
      { cout << "socket state = " << socket->state() << endl; }
    forever
    {
        emit signalSentToServer(QString("->ping<-"));
        wait(3000);
    }
}


void WEBI::connectToServer(QString hostName, quint16 port)
{

    socket->connectToHost(hostName, port);
    socket->waitForConnected(3000);
}


void WEBI::sentToServer(QByteArray& arrBlock)
{
    socket->write(arrBlock);
    socket->flush();
}


void WEBI::readLib()
{

}


/*
 * Слот-метод для отправки данных серверу
*/
void WEBI::slotSentToServer(QString text)
{
//здесь подразумевается, что сокет уже назначен
//и соединение с сервером установлено(TCP)

    if(socket == nullptr) //не назначен
        qDebug() << "Socket was lost in \"slotSentToServer()\"";

    //данные для отправки
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_2);

    out << messagesize_t(0) << QTime::currentTime() << text;

    out.device()->seek(0);
    out << messagesize_t(arrBlock.size() - sizeof(messagesize_t));

    socket->write(arrBlock);
    socket->flush();
}


void WEBI::slotConnected()
{
    qDebug() << "Received the connected() signal.";
}


void WEBI::slotReadyRead()
{
    qDebug() << "Ready read slot";
    QDataStream in(socket);


    in.setVersion(QDataStream::Qt_5_3);
    for(;;)
    {
        if (!m_nNextBlockSize)//если неизвестен размер блока
        {
            //если пришло пустое сообщение
            if (socket->bytesAvailable() < sizeof(messagesize_t)){
                break;
            }
            in >> m_nNextBlockSize;
        }

        //если сообщение пришло не полностью
        if (socket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }

        quint16 message_t = 0;

        QTime   time;
        QString str;

        QByteArray buff;



        in >> message_t;
        if(message_t == 1) {
            qDebug() << "got message with code (" << message_t <<")";
            in >> time >> str;
            qDebug() << time.toString() + " " + str;
        }
        else if(message_t == 2) {
            qDebug() << "got message with code (" << message_t <<")";
            quint32 size = 0;
            in >> size;                //Передача данных в файл производится через буфер buff

            //строгое чтение остатка сообщения
            char buffer8t[size];

            //читаем предполагаемый файл по присланному размеру
            if(auto res = in.readRawData(buffer8t, size); res == 0 || res == -1)
                qDebug() << "incomes txt are empty, considering that size larger";


            QFile newfile("test.txt");
            newfile.open(QIODevice::WriteOnly);
            newfile.write(buffer8t, size);
            newfile.close();
            qDebug() << "Got txt here";

            emit signalSentToServer("Got txt, thanks =)");
        }
        else if(message_t == 3) {
            qDebug() << "DBG got message with code (" << message_t <<")";
            quint32 size = 0;
            in >> size;

            //строгое чтение остатка сообщения
            char* data = nullptr;
            char buffer8t[size];
            qDebug() << "Bytes available to read in socket = " << socket->bytesAvailable();
            in.readBytes(data, size); //создается динамический блок памяти в char* data. Позже нужно освободить.
            memcpy(buffer8t, data, size);
            if(data != nullptr)
                delete[] data; //освобождение памяти после QDataStream::readBytes

            QFile newfile("MyReceivedLIB.dll");
            newfile.open(QIODevice::WriteOnly);
            newfile.write(buffer8t, size);
            newfile.close();

            qDebug() << "Got lib here";

            emit signalSentToServer("Got new dll, thanks =)");
        }

        m_nNextBlockSize = 0;
    }
}


void WEBI::pingToServer()
{
//ping to server
    QByteArray barr;
    QDataStream ds(&barr, QIODevice::WriteOnly);
    ds.setVersion(QDataStream::Qt_5_3);

    ds << quint16(0) << QTime::currentTime() << QString(" <- ping from client");

    //корректировка размера сообщения
    ds.device()->seek(0);
    ds << quint16(barr.size() - sizeof(quint16));

    sentToServer(barr);
}


void WEBI::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
            "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                         "The host not found." :
                         err == QAbstractSocket::RemoteHostClosedError ?
                         "The remote host is closed." :
                         err == QAbstractSocket::ConnectionRefusedError ?
                         "The connection refused error." :
                         QString(socket->errorString())
                        );
    qDebug() << strError;
}


void WEBI::slotTest()
{
    std::cout << "slot test is ok" << std::endl;
}


/*WEBI wait - это костыль,
 * который реализует блокировку Qt-потока внутри самого потока. Для метода at_work.
 * Пока что я хз, как сделать по-другому. Позже надо будет убрать.*/
void WEBI::wait(quint32 time)
{
    std::thread timer([&time]
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
    });

    timer.join();
}


WEBI::~WEBI()
{
    socket->close();
    if(socket) delete socket;
}
