/*!
	\file util_timer.h

	\author Hallgeir Lien

	Contains declaration of the Timer class.
*/

#pragma once

#include <chrono>

namespace Hinage
{
	/*!
		Class for handling timing of events, and other stuff that needs timing.
		It is initialized starting at 0 seconds.
	*/
	class Timer
	{
	private:
		double clock;		//The current time.
		bool started;		//True if the clock is running.
		double lastTick;	//Variable to store the last tick - to calculate the amount of time has passed since last update().

		double getPrecisionTime();

	public:

		/*!
			Constructor. It initializes the timer, setting the clock to zero. The Timer is initially stopped.
		*/
		Timer();
		Timer(bool start);

		/*!
			Updates the timer. Basically it adds the time passed since the last update, to the clock.
			It should be called regularily (for instance every time the game loop loops) to keep it updated.
		*/
		void update();

		/*!
			Starts the timer. It does not reset it, nor does it change the value of the current time if called when the clock is not 0.
		*/
		void start();
		/*!
			Resets the timer to zero. It does not, however, stop the clock.
		*/
		void reset();
		/*!
			Stop the timer.
		*/
		void stop();

		/*!
			Sets the timer to a specified value, in seconds.
		*/
		void setTime(double newtime);

		/*!
			Get the current time.
		*/
		double getTime();

		/*!
			Checks if the timer has reached a specified point.
			\param target The target time, in seconds.
			\note The timer clock does not need to be exactly the target time for this function to interpret it as reached.
			\return true if the specified target time has been reached, and false otherwise.
		*/
		bool hasReached(double target);
	};
}
