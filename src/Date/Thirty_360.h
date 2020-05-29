#ifndef THIRTY_360_H
#define THIRTY_360_H
#include "DayCountCalculator.h"
#include <algorithm>

// Class that inherits from DayCountCalculator
class Thirty_360 : public DayCountCalculator
{
	public:
	static double compute_daycount(const std::tm& from, const std::tm& to);
	static double compute_daycount(const short years, const short months, const short days_from, const short days_to);

    template<class DATE> double operator () (const DATE& start, const DATE& end) const
    {
        // Overload parenthesis operator to return the difference in days
        return compute_daycount(start, end) / 360.0;
    }
};

// Compute the difference in years, months, and days separately because we are supposing there are 30 days in a month
double Thirty_360::compute_daycount(const std::tm& from, const std::tm& to)
{
    auto years = to.tm_year - from.tm_year;
    auto months = to.tm_mon - from.tm_mon;
    return compute_daycount(years, months, from.tm_mday, to.tm_mday);
}

// Compute the difference in days
double Thirty_360::compute_daycount (const short years,	const short months, const short days_from, const short days_to)
{
    // std::max & std::min: returns the greatest and lowest of two values so as to ignore days above 30
    // TODO: por que hay que restar 1 a months
    return (360 * years) + 30 * (months -1) + std::max<short>(0, 30 - days_from) + std::min<short>(30, days_to);
}

#endif
