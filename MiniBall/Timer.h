#ifndef TIMER_H
#define TIMER_H

namespace MiniBall
{
	class Timer
	{
	public:
		Timer();

		static double seconds();
		static double ticks();
		static void pause(double p);
	};
}

#endif   // TIMER_H