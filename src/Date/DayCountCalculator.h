#ifndef DAY_COUNT_CALCULATOR_H
#define DAY_COUNT_CALCULATOR_H

#include <ctime>

class DayCountCalculator
{
  public:
    static std::tm make_tm(int year, int month, int day);
    std::tm generate_tm(std::tm firstTime, double addedTime);
};

std::tm DayCountCalculator::make_tm(int year, int month, int day)
{
    std::tm output ={0};           // Initialize output as a type std::tm
    output.tm_year = year - 1900;  // Year must start in 0 (library ). std::tm.tm_year: years since 1900
    output.tm_mon = month - 1;     // Months must start in 0. std::tm.tm_mon: months since January
    output.tm_mday = day;          // std::tm_mdat: day of the month (1-31)
    return output;

}
// Add time
std::tm DayCountCalculator::generate_tm(std::tm firstTime, double addedTime)
{
    int year = 0;
    int month = 0;
    int day = 0;
    double time = addedTime;  // addedTime must be in years
    while(time>0)
    {
        if(time >= 1)
        {
            year++;           // Update the years (add a year)
            time = time - 1;  // Update the addTime (subtract a year because it has been added)
        }
        else if(time >= (1/12))
        {
            month++;
            time = time - (1/12);  // Subtract a month in year units
        }
        else
        {
            day++;
            time = time - (1/360);  // Subtract a day in year units
        }
    }
    std::tm output ={0};
    output.tm_year = firstTime.tm_year + year;  // Add the num of years in addTime to the firtsTime (actual time)
    output.tm_mon = firstTime.tm_mon + month;
    output.tm_mday = firstTime.tm_mday + day;
    return output;
}
#endif
