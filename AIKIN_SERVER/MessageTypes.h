#pragma once
#include <QTime>
#include <QString>

/* Сообщением здесь является вся совокупность информации,
 * которая передается через сокеты, на прикладном уровне,
 * за одно выполнение методов вроде classname::sendTo...().
 * Подразумевается для приёма-передачи сообщений
 * использовать контейнеры определенные ниже в этом заголовочном файле.
 * Полагаю, применение таких условных протоколов для сообщений
 * сделает код методов приема-передачи более стандартизованным.*/


/* Коды для условного определения типов для входных данных. */
enum DataType
{
  //целочисленные знаковые
    dt_char,
    dt_short,
    dt_int,

  //целочисленные беззнаковые
    dt_uint8_t,
    dt_uint16_t,
    dt_uint32_t,
    dt_uint64_t,

  //вещественные
    dt_float,
    dt_double,
    dt_ll_double,
};


using messagesize_t = uint32_t; //тип для обозначения размера в байтах передаваемого сообщения
using filesize_t    = uint32_t; //тип для обозначения размера в байтах передаваемого файла


/* Коды сообщений. Список может быть расширен. */
enum message_t
{
    null_msgt,      /*0*/ /* Зарезервирован нуль-код для особых случаев. */
    String_msgt,    /*1*/ /* Код передачи строки (QString). */
    TextFile_msgt,  /*2*/ /* Код передачи текстового файла. */
    DllFile_msgt,   /*3*/ /* Код передачи объектного файла динамической библиотеки. */
    ProcData_msgt   /*4*/ /* Код передачи данных с их описанием, а также конкретизацией процедуры. */
};


/*Message*/
/* АБК для обобщения интерфейса контейнеров сообщений.*/
class Message /* АБК */
{
    messagesize_t _messagesize;
    message_t _type;

public:
    /*forSend - слияние всех данных пакета в массив байт
     * для последующей записи в сокет*/
    virtual QByteArray forSend() const =0;
};


/*File_message*/
/* АБК для обобщения интерфейса контейнеров сообщений с файлами.*/
class File_message
        : public virtual Message
{
    filesize_t _fsize;
    QByteArray _file;

public:
    File_message(message_t, filesize_t fsize, QByteArray file); //do it
    virtual QByteArray forSend() const;                         //do it
};


/*String_message*/
/*Специализированный контейнер для сообщения
 * с передачей строки.*/
class String_message
        : public Message
{
    QTime _time;
    QString _string;


};


/*TextFile_message*/
/*Специализированный контейнер для сообщения
 * с передачей текстового файла.*/
class TextFile_message
        : public File_message
{

};


/*DllFile_message*/
/*Специализированный контейнер для сообщения
 * с передачей объектного файла динамической библиотеки. */
class DllFile_message
        : public File_message
{

};


/*ProcData_message*/
/*DataPackage - контейнер для представления входных данных. */
class DataPackage
        : public File_message
{
    DataType _dataType;
};

/*Специализированный контейнер для сообщения
 * с передачей данных вместе с их описанием,
 * а также конкретизацией вызываемой процедуры динамической библиотеки.*/
class ProcData_message
        : public Message
{
    QString     _libName;
    QString     _procName;
    DataPackage _dataPackage;

};


class ArchitectState_data_message
        /*: наследование от АБК*/
{
    /*определение протокола передачи конфигов*/
};
