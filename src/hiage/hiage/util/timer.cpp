
#include "timer.h"
//#include <sys/time.h>
#include <chrono>

using namespace hiage;

Timer::Timer(bool start)
{
    clock = 0;
    started = start;

    //set last tick to current time
    lastTick = getPrecisionTime();
}

Timer::Timer()
{
    clock = 0;
    started = false;

    //set last tick to current time
    lastTick = getPrecisionTime();
}

//update the timer. simply adding the time since last update to the clock variable
void Timer::update()
{

    double tick1, tick2;

    tick1 = lastTick;
    tick2 = getPrecisionTime();

    lastTick = tick2;


    if (tick1 > tick2)
    {
        tick1 = 1 - tick1;
        if (tick1 > tick2)
        {
            double temp = tick1;
            tick1 = tick2;
            tick2 = temp;
        }
    }

    clock += tick2 - tick1;

}

//start the timer (but does not reset)
void Timer::start()
{
    if (!started)
    {
        started = true;
        auto start = std::chrono::high_resolution_clock::now();

        auto foo = start.time_since_epoch();
        
        lastTick = getPrecisionTime();
    }
}

//reset the timer to 0
void Timer::reset()
{
    clock = 0;
    lastTick = getPrecisionTime();
}

//stop the timer
void Timer::stop()
{
    if (started)
    {
        started = false;
    }
}

//get the current time in seconds since the timer started
double Timer::getTime()
{
    if (started)
        update();

    return clock;
}

//check if the timer has reached the target time
bool Timer::hasReached(double target)
{
    if (started)
        update();

    return (clock >= target);
}

//set the time (for instance to initialize it to another value than 0)
void Timer::setTime(double newtime)
{
	clock = newtime;
}

double Timer::getPrecisionTime()
{
    auto start = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(
        start.time_since_epoch()).count() / 1000000.0;
}


