#include <QCoreApplication>


#include "Agent.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    Agent agent;
    agent.exec();

    return a.exec();
}
