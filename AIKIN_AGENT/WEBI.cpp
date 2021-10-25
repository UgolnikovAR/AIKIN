#include "WEBI.h"

#include <tuple>

#include <QDebug>
#include <QByteArray>
#include <QDataStream>
#include <QTime>
#include <QTimer>
#include <QFile>

#include <QHostAddress>
#include <QSocketNotifier>

WEBI::WEBI(Spotter* pspotter)
    :QThread(nullptr)
    ,sma_pspotter(pspotter)
{
    sw_pthread = new QThread;
    _socket = new QTcpSocket();
    _sworker = new SocketWorker(_socket, this);
    _sworker->moveToThread(sw_pthread);

    connect(_sworker, SIGNAL(registerDll(QString)), sma_pspotter, SLOT(slotRegDll(QString)));

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
    sw_pthread->start();

    emit signalSentToServer(QString("Signal Call from run()"));

    using namespace std;
    cout << "WEBI in work" << endl;

    at_work();
}


Spotter* WEBI::spotter() {return sma_pspotter;}


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


void WEBI::connectToServer()
{
    /*Здесь производится проверка состояний сокета относительно подключения.*/
    if(_socket->state() == QAbstractSocket::ConnectedState)
        return;
    if(_socket->state() == QAbstractSocket::UnconnectedState)
    {
        connectToServer(autoHostName(), autoNPort());
        _socket->waitForConnected(3000);
    }
    /*.. ..*/
          /*Предполагается обрабатывать все состояния сокета.*/
}


void WEBI::connectToServer(QString hostName, quint16 port)
{
    if(_socket->state() == QAbstractSocket::UnconnectedState) {
        qDebug() << "Trying to connect to " << hostName << " at port" << port;
        _socket->connectToHost(hostName, port);
        _socket->waitForConnected(3000);
    }
}


/*Запись данных в сокет и отправка*/
void WEBI::sentToServer(QByteArray& arrBlock)
{
    _socket->write(arrBlock);
    _socket->waitForBytesWritten(3000);
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


QString WEBI::autoHostName()
{
    /*Здесь предполагается автоматическое определение имени хоста-сервера*/
    return QString("localhost");
}
NPort   WEBI::autoNPort()
{
    /*Здесь предполагается автоматическое определение номера порта хоста-сервера*/
    return NPort(2323);
}


WEBI::~WEBI()
{
    _socket->close();
    if(_socket) delete _socket;
}
//==============================================================================


bool SocketWorker::connectToServer()
{
    /*Здесь производится проверка состояний сокета относительно подключения.*/
    if(_socket->state() == QAbstractSocket::ConnectedState)
        return true;
    if(_socket->state() == QAbstractSocket::UnconnectedState)
    {
        connectToServer(autoHostName(), autoNPort());
        _socket->waitForConnected(5000);
    }
    /*.. ..*/
          /*Предполагается обрабатывать все состояния сокета.*/

    /*Если так и не получилось подключиться, тогда просто выходим.*/
    if(_socket->state() == QAbstractSocket::ConnectedState)
        return true;
    else
        return false;
}


void SocketWorker::connectToServer(QString hostName, quint16 port)
{
    if(_socket->state() == QAbstractSocket::UnconnectedState) {
        qDebug() << "Trying to connect to " << hostName << " at port" << port;
        _socket->connectToHost(hostName, port);
        _socket->waitForConnected(5000);
    }
}


QString SocketWorker::autoHostName()
{
    /*Здесь предполагается автоматическое определение имени хоста-сервера*/
    return QString("localhost");
}
NPort   SocketWorker::autoNPort()
{
    /*Здесь предполагается автоматическое определение номера порта хоста-сервера*/
    return NPort(2323);
}


void SocketWorker::slotSentToServer(QString text)
{
        if(_socket == nullptr) //не назначен
            qDebug() << "Socket was lost in \"slotSentToServer()\"";

        /*Попытка подключения к серверу,
         * Если удачно - отправляем данные,
         * Иначе пропускаем этот пинг.*/
        if(connectToServer())
        {
            /*Подготовка данных к отправке*/
            QByteArray arrBlock;
            QDataStream out(&arrBlock, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_5_2);

            out << messagesize_t(0) << QTime::currentTime() << text;

            out.device()->seek(0);
            out << messagesize_t(arrBlock.size() - sizeof(messagesize_t));

            /*Отправка данных*/
            _socket->write(arrBlock);
            _socket->waitForBytesWritten(3000);
            _socket->flush();
        }

        /*Закрытие сокета*/
        /*Здесь обязательно нужна проверка: передает ли сейчас сервер данные*/
        //_socket->close();
}


void SocketWorker::slotConnected()
{
    qDebug() << "Received the connected() signal.";
}
void SocketWorker::slotReadyRead()
{
    //qDebug() << "Ready read slot in SocketWorker.";
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

        messagetype_t message_t = messagetype_t(0);

        QTime   time;
        QString str;

        QByteArray buff;

        /*Передача кортежа ссылок на локальные ОД в субпроцедуры*/
        auto local_data = std::make_tuple(std::ref(message_t),
                                          std::ref(in),
                                          std::ref(time),
                                          std::ref(str)
                                          );

        in >> message_t;
        if(message_t == String_msgt)
        {
            string_msgt_proc(local_data);
        }
        else if(message_t == TextFile_msgt)
        {
            textFile_msgt_proc(local_data);
        }
        else if(message_t == DllFile_msgt)
        {
            dllFile_msgt_proc(local_data);
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


void SocketWorker::string_msgt_proc(subproc_data d)
{
    auto& [message_t, in, time, str] = d;

    qDebug() << "got message with code (" << message_t <<")";
    in >> time >> str;
    qDebug() << time.toString() + " " + str;
}


void SocketWorker::textFile_msgt_proc(subproc_data d)
{
    auto& [message_t, in, time, str] = d;
    qDebug() << "DBG got message with code (" << message_t <<")";
    quint32 size = 0;
    in >> size;

    /*строгое чтение остатка сообщения*/
    char* data = nullptr;
    char buffer8t[size];
    qDebug() << "Bytes available to read in socket = " << _socket->bytesAvailable();
    in.readBytes(data, size); //создается динамический блок памяти в char* data. Позже нужно освободить.
    memcpy(buffer8t, data, size);
    if(data != nullptr)
        delete[] data; //освобождение памяти после QDataStream::readBytes


    QString libname = _pwebi->spotter()->newDllName();

    qDebug() << "Got lib here";
    emit registerDll(libname);

    /*Создается файл и происходит запись библиотеки*/
    QFile newfile(libname);
    newfile.open(QIODevice::WriteOnly);
    newfile.write(buffer8t, size);
    newfile.close();

    emit slotSentToServer("Got new dll, thanks =)");


}


void SocketWorker::dllFile_msgt_proc(subproc_data d)
{
    auto& [message_t, in, time, str] = d;

    qDebug() << "DBG got message with code (" << message_t <<")";
    quint32 size = 0;
    in >> size;

    /*строгое чтение остатка сообщения*/
    char* data = nullptr;
    char buffer8t[size];
    qDebug() << "Bytes available to read in socket = " << _socket->bytesAvailable();
    in.readBytes(data, size); //создается динамический блок памяти в char* data. Позже нужно освободить.
    memcpy(buffer8t, data, size);
    if(data != nullptr)
        delete[] data; //освобождение памяти после QDataStream::readBytes


    QString libname = _pwebi->spotter()->newDllName();

    qDebug() << "Got lib here";
    emit registerDll(libname);

    /*Создается файл и происходит запись библиотеки*/
    QFile newfile(libname);
    newfile.open(QIODevice::WriteOnly);
    newfile.write(buffer8t, size);
    newfile.close();

    emit slotSentToServer("Got new dll, thanks =)");
}
