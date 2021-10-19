#include "WEBI.h"
#include <QDebug>
#include <QByteArray>
#include <QDataStream>
#include <QTime>
#include <QTimer>
#include <QFile>

#include <QHostAddress>
#include <QSocketNotifier>

WEBI::WEBI()
    :QThread(nullptr)
{
    _sw_thread = new QThread;
    _socket = new QTcpSocket();
    _sworker = new SocketWorker(_socket);
    _sworker->moveToThread(_sw_thread);


    connect(_socket, SIGNAL(connected()), _sworker, SLOT(slotConnected()), Qt::DirectConnection);
    connect(_socket, SIGNAL(readyRead()), _sworker, SLOT(slotReadyRead()), Qt::DirectConnection);
    connect(_socket, SIGNAL(disconnected()), _sworker, SLOT(slotDisconnected()), Qt::DirectConnection);


    connect(this, SIGNAL(signalSentToServer(QString)), _sworker, SLOT(slotSentToServer(QString)), Qt::QueuedConnection);

    connectToServer("localhost", 2323);
}

/*Запуск потока WEBI*/
void WEBI::run()
{
    /*Запуск потока обработки сигналов сокета WEBI*/
    _sw_thread->start();

    emit signalSentToServer(QString("Signal Call from run()"));

    using namespace std;
    cout << "WEBI in work" << endl;

    at_work();
}


void WEBI::at_work()
{
    using namespace std;

    if(_socket->state() == QAbstractSocket::ConnectedState)
        cout << "connected!" << endl;
    else if(_socket->state() == QAbstractSocket::ConnectingState)
        cout << "connecting..." << endl;
    else if(_socket->state() == QAbstractSocket::UnconnectedState)
        cout << "unconnected" << endl;
    else
      { cout << "socket state = " << _socket->state() << endl; }
    forever
    {
        emit signalSentToServer(QString("->ping<-"));
        wait(3000);
    }
}


void WEBI::connectToServer(QString hostName, quint16 port)
{
    if(_socket->state() == QAbstractSocket::UnconnectedState) {
        qDebug() << "Trying to connect to " << hostName << " at port" << port;
        _socket->connectToHost(hostName, port);
        _socket->waitForConnected(3000);
    }
}


void WEBI::sentToServer(QByteArray& arrBlock)
{
    _socket->write(arrBlock);
    _socket->flush();
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
    _socket->close();
    if(_socket) delete _socket;
}
//==============================================================================


void SocketWorker::slotSentToServer(QString text)
{
    //здесь подразумевается, что сокет уже назначен
    //и соединение с сервером установлено(TCP)

        if(_socket == nullptr) //не назначен
            qDebug() << "Socket was lost in \"slotSentToServer()\"";

        //данные для отправки
        QByteArray arrBlock;
        QDataStream out(&arrBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_2);

        out << messagesize_t(0) << QTime::currentTime() << text;

        out.device()->seek(0);
        out << messagesize_t(arrBlock.size() - sizeof(messagesize_t));

        _socket->write(arrBlock);
        _socket->flush();
}

void SocketWorker::slotConnected()
{
    qDebug() << "Received the connected() signal.";
}
void SocketWorker::slotReadyRead()
{
    qDebug() << "Ready read slot in SocketWorker.";
    QDataStream in(_socket);

    in.setVersion(QDataStream::Qt_5_3);
    for(;;)
    {
        if (!_nNextBlockSize)//если неизвестен размер блока
        {
            //если пришло пустое сообщение
            if (_socket->bytesAvailable() < sizeof(messagesize_t)){
                break;
            }
            in >> _nNextBlockSize;
        }

        //если сообщение пришло не полностью
        if (_socket->bytesAvailable() < _nNextBlockSize) {
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

            emit slotSentToServer("Got txt, thanks =)");
        }
        else if(message_t == 3) {
            qDebug() << "DBG got message with code (" << message_t <<")";
            quint32 size = 0;
            in >> size;

            //строгое чтение остатка сообщения
            char* data = nullptr;
            char buffer8t[size];
            qDebug() << "Bytes available to read in socket = " << _socket->bytesAvailable();
            in.readBytes(data, size); //создается динамический блок памяти в char* data. Позже нужно освободить.
            memcpy(buffer8t, data, size);
            if(data != nullptr)
                delete[] data; //освобождение памяти после QDataStream::readBytes

            QFile newfile("MyReceivedLIB.dll");
            newfile.open(QIODevice::WriteOnly);
            newfile.write(buffer8t, size);
            newfile.close();

            qDebug() << "Got lib here";

            emit slotSentToServer("Got new dll, thanks =)");
        }

        _nNextBlockSize = 0;
    }
}


void SocketWorker::slotDisconnected()
{
    qDebug() << "..disconnected";
}


void SocketWorker::pingToServer()
{
    slotSentToServer(" <- ping from client");
}
