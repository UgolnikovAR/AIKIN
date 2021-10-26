#include "Spotter.h"
#include "AIKIN_utility.h"

#include <QLibrary>

QVector<QString> Spotter::_dllRegistr{};

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
    tsout << "slotRegDll: " << libname << " registered.";

    Spotter::_dllRegistr.push_back(libname);
}
