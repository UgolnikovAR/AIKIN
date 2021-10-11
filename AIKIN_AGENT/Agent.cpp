#include "Agent.h"

Agent::Agent()
 : //WEBI(), olds
   Spotter(),
   Processor()
{
    proc = Spotter::form_procedure();

}


void Agent::exec()
{
    webi.start(QThread::NormalPriority);

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
        cout << "Agent : ping " << i << endl;
    }
    cout << "Agent exit" << endl;
}

