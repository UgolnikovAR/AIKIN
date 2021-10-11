#pragma once
#include <iostream>
#include <thread>
#include <chrono>

#include "Procedure.h"

/* class Processor. Модуль. Вычисления.
 * Получает задание из вызывающей процедуры. Выполняет и записывает результат в State.*/
//вычислительный модуль
class Processor
{
public:
    Procedure proc;

public:
    Processor() {};

    /* Processor::run - Выполнение вычислений.
     * Записывает результат в State, помечает флаг (Result_ok)
    */
    void run(); //thread creats

private:
    static void exec(Processor*);
    static void at_work(Processor*);
};
