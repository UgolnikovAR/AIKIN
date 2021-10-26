#include "WEBI.h"
#include "AIKIN_utility.h"

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

    InThreadSocketWorker* _socketAdapter = new InThreadSocketWorker(_socket);
    _socketAdapter->moveToThread(_socket->thread());

    _sworker = new SocketWorker(_socket, this);
    _sworker->moveToThread(sw_pthread);

    //пересылка сигнала передачи данных сокету через адаптер
    connect(_sworker, SIGNAL(sendData(QByteArray*)), _socketAdapter, SLOT(slotSendData(QByteArray*)));

    //сигнал на регистрацию библиотеки
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
    tsout << "WEBI in work";

    at_work();
}


Spotter* WEBI::spotter() {return sma_pspotter;}


void WEBI::at_work()
{
    using namespace std;

    if(_socket->state() == QAbstractSocket::ConnectedState)
        tsout << "connected!";
    else if(_socket->state() == QAbstractSocket::ConnectingState)
        tsout << "connecting...";
    else if(_socket->state() == QAbstractSocket::UnconnectedState)
        tsout << "unconnected";
    else
      { tsout << "socket state = " << _socket->state(); }
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
        tsout << "Trying to connect to " << hostName << " at port" << port;
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
    if(_psocket->state() == QAbstractSocket::ConnectedState)
        return true;
    if(_psocket->state() == QAbstractSocket::UnconnectedState)
    {
        connectToServer(autoHostName(), autoNPort());
        _psocket->waitForConnected(5000);
    }
    /*.. ..*/
          /*Предполагается обрабатывать все состояния сокета.*/

    /*Если так и не получилось подключиться, тогда просто выходим.*/
    if(_psocket->state() == QAbstractSocket::ConnectedState)
        return true;
    else
        return false;
}


void SocketWorker::connectToServer(QString hostName, quint16 port)
{
    if(_psocket->state() == QAbstractSocket::UnconnectedState) {
        tsout << "Trying to connect to " << hostName << " at port" << port;
        _psocket->connectToHost(hostName, port);
        _psocket->waitForConnected(5000);
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
        if(_psocket == nullptr) //не назначен
            tsout << "Socket was lost in \"slotSentToServer()\"";

        /*Попытка подключения к серверу,
         * Если удачно - отправляем данные,
         * Иначе пропускаем этот пинг.*/
        if(connectToServer())
        {
            /*Подготовка данных к отправке*/
            QByteArray* arrBlock = new QByteArray;
            QDataStream out(arrBlock, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_5_2);

            out << messagesize_t(0) << QTime::currentTime() << text;

            out.device()->seek(0);
            out << messagesize_t(arrBlock->size() - sizeof(messagesize_t));

            /*Отправка данных*/
            emit sendData(arrBlock);
            /*
            _psocket->write(arrBlock);
            _psocket->waitForBytesWritten(3000);
            _psocket->flush();*/
        }

        /*Закрытие сокета*/
        /*Здесь обязательно нужна проверка: передает ли сейчас сервер данные*/
        //_socket->close();
}


void SocketWorker::slotConnected()
{
    tsout << "Received the connected() signal.";
}
void SocketWorker::slotReadyRead()
{
    //tsout << "Ready read slot in SocketWorker.";
    QDataStream in(_psocket);

    in.setVersion(QDataStream::Qt_5_3);
    for(;;)
    {
        if (!_nNextBlockSize)//если неизвестен размер блока
        {
            //если пришло пустое сообщение
            if (_psocket->bytesAvailable() < sizeof(messagesize_t)){
                break;
            }
            in >> _nNextBlockSize;
        }

        //если сообщение пришло не полностью
        if (_psocket->bytesAvailable() < _nNextBlockSize) {
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
    tsout << "..disconnected";
}


void SocketWorker::pingToServer()
{
    slotSentToServer(" <- ping from client");
}


void SocketWorker::string_msgt_proc(subproc_data d)
{
    auto& [message_t, in, time, str] = d;

    tsout << "got message with code (" << message_t <<")";
    in >> time >> str;
    tsout << time.toString() + " " + str;
}


void SocketWorker::textFile_msgt_proc(subproc_data d)
{
    auto& [message_t, in, time, str] = d;
    tsout << "DBG got message with code (" << message_t <<")";
    quint32 size = 0;
    in >> size;

    /*строгое чтение остатка сообщения*/
    char* data = nullptr;
    char buffer8t[size];
    tsout << "Bytes available to read in socket = " << _psocket->bytesAvailable();
    in.readBytes(data, size); //создается динамический блок памяти в char* data. Позже нужно освободить.
    memcpy(buffer8t, data, size);
    if(data != nullptr)
        delete[] data; //освобождение памяти после QDataStream::readBytes


    QString libname = _pwebi->spotter()->newDllName();

    tsout << "Got lib here";
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

    tsout << "DBG got message with code (" << message_t <<")";
    quint32 size = 0;
    in >> size;

    /*строгое чтение остатка сообщения*/
    char* data = nullptr;
    char buffer8t[size];
    tsout << "Bytes available to read in socket = " << _psocket->bytesAvailable();
    in.readBytes(data, size); //создается динамический блок памяти в char* data. Позже нужно освободить.
    memcpy(buffer8t, data, size);
    if(data != nullptr)
        delete[] data; //освобождение памяти после QDataStream::readBytes


    QString libname = _pwebi->spotter()->newDllName();

    tsout << "Got lib here";
    emit registerDll(libname);

    /*Создается файл и происходит запись библиотеки*/
    QFile newfile(libname);
    newfile.open(QIODevice::WriteOnly);
    newfile.write(buffer8t, size);
    newfile.close();

    emit slotSentToServer("Got new dll, thanks =)");
}


/*Слот принимает динамический объект, затем удаляет его*/
void InThreadSocketWorker::slotSendData(QByteArray* data)
{
    if(data != nullptr) {
        _psocket->write(*data);
        _psocket->waitForBytesWritten(3000);
        _psocket->flush();

        delete data;
    }
}
