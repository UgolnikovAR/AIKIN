#pragma once

#include <QString>
#include <QLibrary>
#include <QDebug>

/*class Result - определение структуры данных - результата выполнения процедуры */
class Result {/*...*/};

/*class Data   - определение структуры данных - исходных данных для процедуры */
class Data {/*...*/};


/* Предполагаю, что процедура будет резервировать память под Result.
 * В процессе упаковки данных в WEBI::Packet, можно ту память освобождать.*/

//typedef Result* (*Procedure_func) (Data*);
typedef void (*Procedure_func) (); //Пока не определил Data и Result, потесчу пустышку.

void nullproc(); //пустышка для инициализации по умолчанию.


class Procedure
{
    Procedure_func pProcedure = nullproc;

public:
    Procedure() {}
    Procedure(QLibrary& lib, const char* funcName)
    {
    //Извлекаем указатель на процедуру из либы по имени        
        if(pProcedure = Procedure_func(lib.resolve(funcName))) {}
        else
        { //Если в либе не найдена процедура отправляем warning
            qDebug() << "Procedure constructor was'nt found lib's function " + QString(funcName);
            //и ставим пустышку
            pProcedure = nullproc;
        }
    }

    void operator()() {     //Для теста процедуры без параметров
        return pProcedure();
    }
/*
    Result* operator()(Data* data) {
        return pProcedure(data);
    }
    */
};


