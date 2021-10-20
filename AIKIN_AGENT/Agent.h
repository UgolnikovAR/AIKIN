#pragma once

#include <iostream>
#include <thread>
#include <chrono>


#include "Processor.h"
#include "Spotter.h"
#include "WEBI.h"

#include "Procedure.h"

static const int PERIOD_DELAY  = 5;
static const int PERIOD        = 1;


//class Spotter_data  {/*...*/}; //olds

/*Класс, описывающий архитектурное состояние агента*/
class ArchitectureCondition
{
    /* Флаг, устанавливающийся в момент, когда не установлено флагов,
     * говорящих о критических ситуациях, приводящих к некорректной работе агента.*/
    bool allConditionsIsOkay;

    /* Флаги архитектурного состояния агента.
     * Отражают результаты методов Spotter.
     * Отсюда их состояние может получать любой модуль агента, посредством геттеров.*/
    bool flag_agentHalt;
    bool flag_agentUpdate;
    bool flag_agentUpdateDLL;
    bool flag_agentResume;

    /*WEBI*/
    bool flag_agentURLCheckuot;
};


/* class Agent:
 * Основной класс программы, который запускает два рабочих потока.
 * Основным потоком является процедура Agent::exec,
 * которая работает в главном потоке программы.
 * */
class Agent : //private WEBI,
              public Spotter,
              private Processor
{
    WEBI _webi;

    static ArchitectureCondition _archCondition;

    void at_work();

public:
    Agent();
    void exec();
};
