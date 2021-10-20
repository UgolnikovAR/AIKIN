#include "Spotter.h"
#include <QLibrary>

#define procName "dllsays"
static const QString dllName{"AIKIN_LIB"};


Procedure Spotter::form_procedure()
{
    QLibrary lib(dllName);
    return Procedure(lib, procName);
}


QString Spotter::newDllName()
{
    /* Здесь подразумевается реализация механизма
     * генерации уникальных имен для библиотек.*/
    return QString("ReceivedLib.dll");
}


void Spotter::slotRegDll(QString libname)
{
    /*Добавление названия библиотеки в вектор*/
    qDebug() << "slotRegDll: register " << libname;

    _dllRegistr.push_back(libname);
}
