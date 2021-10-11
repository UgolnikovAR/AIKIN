#include "Spotter.h"
#include <QLibrary>

#define procName "dllsays"
static const QString dllName{"AIKIN_LIB"};


Procedure Spotter::form_procedure()
{
    QLibrary lib(dllName);
    return Procedure(lib, procName);
}


void Spotter::slotRegDll(QString)
{
    /*формирование и добавление библиотеки в вектор*/
}
