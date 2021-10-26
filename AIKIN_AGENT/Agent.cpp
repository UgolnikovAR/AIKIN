#include "Agent.h"
#include "AIKIN_utility.h"

Agent::Agent()
 :  Spotter()
   ,Processor()
   ,_webi((Spotter*)(this))
{
    proc = Spotter::form_procedure();
}


void Agent::exec()
{
    _webi.start(QThread::NormalPriority);

    Processor::run();

    //Agent::at_work();

}


void Agent::at_work()
{
    using namespace std;
    using namespace chrono;
    for(int i=0; 1; i++)
    {
        this_thread::sleep_for(milliseconds(PERIOD_DELAY * PERIOD * 1000));
        tsout << "Agent : ping " << i;
    }
    tsout << "Agent exit";
}
