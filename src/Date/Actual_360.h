#ifndef ACTUAL_360_H
#define ACTUAL_360_H
#include "DayCountCalculator.h"

// Class  that inherits from DayCountCalculator
class Actual_360 : public DayCountCalculator
{
	public:
		static double compute_daycount(const std::tm& from, const std::tm& to);

		template<class DATE>  // Date template because it could be a std:tm object or any other date object type
		double operator () (const DATE& start, const DATE& end) const
		{
            // Overload parenthesis operator to compute the number of days between the end and the start
			return compute_daycount(start, end)/ 360.0;
		}
};

// Compute the number of days between the end and the start
double Actual_360::compute_daycount(const std::tm& from_date, const std::tm& to_date)
{
    // Initialize the objects by reference (both are std::tm type)
	std::tm from(from_date);
	std::tm to(to_date);

	// Convert from std::tm (date) to std::time_t holding the number of seconds (for computing difference)
	auto from_time = std::mktime(&from);
	auto to_time = std::mktime(&to);

	// Divide by the number of seconds in a day
	constexpr const int seconds_per_day = 60*60*24;  // constexpr: evaluate the value of the function at compile time.

	return std::difftime(to_time, from_time) / (seconds_per_day); // std::difftime: diff between two times in seconds
}
#endif
