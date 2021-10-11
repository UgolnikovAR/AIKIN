#include "Processor.h"
#include "Spotter.h"

#include <QDebug>

void Processor::exec(Processor* p)
{
    using namespace std;
    cout << "Processor in work" << endl;

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
    cout << "Processor exit" << endl;
}


void Processor::run()
{
    std::thread t (exec, this);
    t.detach();
}
