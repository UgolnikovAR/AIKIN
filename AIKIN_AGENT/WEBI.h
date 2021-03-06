#pragma once


/* Реализация сетевого интерфейса:
 * LISTEN(ожидание)     - Блокировка, ожидание входящего соединения
 * CONNECT(соединение)  - Установка соединения с ожидающим объектом
 * ACCEPT(прием)        - Прием входящего соединения от объекта того же уровня
 * RECEIVE(прием)       - Блокировка, ожидание входящего сообщения
 * SEND(отправка)       - Отправка сообщения ожидающему объекту того же уровня
 * DISCONNECT(разрыв)   - Разрыв соединения
*/


#include <iostream>
#include <thread>
#include <chrono>

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QDebug>

#include "Spotter.h"
#include "../AIKIN_SERVER/MessageTypes.h"

class SocketWorker;

/* class WEBI. Модуль. Сетевой интерфейс.
 * Ожидает приём пакета.
 * При получении, расшифровывает пакет и формирует объект ArchitectState_data.
 * Объект ArchitectState_data передается в область вызывающей процедуры (Поток-1).
 */
//Интерфейс сетевого модуля

using messagesize_t = quint32;
using messagetype_t = quint16;
using NPort = quint32;
class WEBI : public QThread
{
    Q_OBJECT //для сигнально-слотового соединения

public:
    explicit WEBI(Spotter*);
            ~WEBI();

    /* WEBI::run  -  Наблюдение за сетью,
     * вызов методов Spotter.*/
    void run(); //thread creates
    void sentToServer(QByteArray&);
    Spotter* spotter();

private: //Процедуры работы с сетью
    /*Попытка соединения с установленным сервером по-умолчанию*/
    void connectToServer();
    void connectToServer(QString, quint16);

private slots:
    /*.. ..*/

private: //Организационные процедуры
    //static void exec(WEBI*); olds
    void at_work();
    void wait(quint32 msec);

private:
    SocketWorker* _sworker;
    QThread* sw_pthread;
    QTcpSocket* _socket;

    messagesize_t     _nNextBlockSize = 0;
    Spotter* sma_pspotter;

private: //вспомогательные методы
    QString autoHostName();
    NPort   autoNPort();

signals:
    void signalSentToServer(QString);
    void sigTest();
};

/*кортеж ссылок для сообщения локальных ОД вспомогательным процедурам*/
using subproc_data = std::tuple<messagetype_t&, QDataStream&, QTime&, QString&>;
/*Класс обработки сигналов сокета WEBI*/
class SocketWorker
        : public QObject
{
    Q_OBJECT
    QTcpSocket* _psocket;

public:
    SocketWorker(QTcpSocket*sock, WEBI* pwebi)
        : _psocket(sock)
        , _pwebi(pwebi){}
    virtual ~SocketWorker(){};

private: //Процедуры работы с сетью
    /*Попытка соединения с установленным сервером по-умолчанию*/
    bool connectToServer();
    void connectToServer(QString, quint16);

private: //вспомогательные методы
    QString autoHostName();
    NPort   autoNPort();

public slots:
    void slotSentToServer(QString);
    void slotConnected();
    void slotReadyRead();
    void slotDisconnected();
    void pingToServer();

private: //вспомогательные процедуры для обработки слотов
    void string_msgt_proc(subproc_data);
    void textFile_msgt_proc(subproc_data);
    void dllFile_msgt_proc(subproc_data);

private:
    messagesize_t _nNextBlockSize = 0;
    WEBI* _pwebi;

signals:
    void registerDll(QString);
    void sendData(QByteArray*);
};

/*Класс-адаптер, позволяющий обойти ограничение
 *  на управление QSocketNotifier между потоками.
 *  Здесь используется сигнально-слотовое соединение между потоками.*/
class InThreadSocketWorker
        : public QObject
{
    Q_OBJECT

    QTcpSocket* _psocket;
public:
    InThreadSocketWorker(QTcpSocket* s)
        : _psocket(s)
    {}

public slots:
    void slotSendData(QByteArray*);
};
