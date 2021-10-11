#pragma once

#include "Procedure.h"

#include <QLibrary>

/* class Spotter. Модуль. Наблюдатель и корректировщик.
 * Получает объект Spotter_data, выполняет декомпозицию данных и запись в поля State.
 * В вызывающей процедуре запускается выделенный поток std::thread(Spotter::control)
 */

class Spotter
{
    QVector<QString> reg_libs;
public:
    Spotter() {};

    /* form_procedure - устанавливает процедуру в модуль processor.*/
    Procedure form_procedure();
    /* Spotter::control  -  Наблюдение за состоянием State,
     * корректировка состояния.
    */

private:
//Процедуры для работы с состоянием агента
    void agent_halt()   {};
    void agent_resume() {};
    void state_update() {};
    void URL_checkout() {};

//Процедуры для работы с динамической библиотекой
public slots:
    void slotRegDll(QString);



};
