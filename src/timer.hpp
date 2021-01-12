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

 public:
		Timer(): start_time_(take_time_stamp()) {}

		void reset() {
			start_time_ = take_time_stamp();
		}

		double elapsed() const {
			return double((take_time_stamp() - start_time_) * 1e-9);
		}

    std::uint64_t elapsed_nanoseconds() const {
      return take_time_stamp() - start_time_;
    }

 protected:
  static std::uint64_t take_time_stamp() {
    return std::uint64_t(
      std::chrono::high_resolution_clock::now().time_since_epoch().count()
    );
  }

 private:
  std::uint64_t start_time_;

};

#endif
