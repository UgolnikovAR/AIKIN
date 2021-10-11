#include "aikin_lib.h"
#include <QDebug>

void dllsays()
{
    qDebug() << "dll is working! =)";
}

AIKIN_LIB::AIKIN_LIB()
{
    qDebug() << "lib initialized";
}
