#include "Processor.h"
#include "Spotter.h"
#include "AIKIN_utility.h"

#include <QDebug>

void Processor::exec(Processor* p)
{
    using namespace std;
    tsout << "Processor in work";

    at_work(p);
}


void Processor::at_work(Processor* p)
{
    using namespace std;
    using namespace chrono;

    while(true)
    {
        this_thread::sleep_for(milliseconds(300*10));
        p->proc();
    }
    tsout << "Processor exit";
}


void Processor::run()
{
    std::thread t (exec, this);
    t.detach();
}
