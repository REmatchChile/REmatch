#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

class Timer
/*
Usage:
		Timer t;

		// Code to time goes here

		std::cout << "Time elapsed: " << t.elapsed() << " seconds\n"
*/
{
	private:
		using clock_t = std::chrono::high_resolution_clock;
		using second_t = std::chrono::duration<double, std::ratio<1> >;

		std::chrono::time_point<clock_t>  m_beg;

	public:
		inline Timer(): m_beg(clock_t::now()) {};

		inline void reset() { 
			m_beg = clock_t::now();
		}

		inline double elapsed() const {
			return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
		}
};

#endif
