#ifndef SQF_ZEROCOUPON_H
#define SQF_ZEROCOUPON_H

#include <ctime>
#include <cmath>

template <class T>
class ZeroCoupon
{
    private:
        double timeInYears;             // Time in years (calculated on the constructor)
        double forward;                 // Forward rate
        std::tm date;                   // Date object
        double zeroCouponInterestRate;  // Interest rate
    public:
        ZeroCoupon(std::tm _date, double _zeroCouponInterestRate, T dayConventionObject, std::tm initialDate);

        // Setter: Called in ZeroCouponYieldCurve to set the forward of the ith zeroCoupon object
        // in the zeroCouponVector
        void setForward(double timeInYearsBefore, double interestRateBefore, double numOfPeriodsPerYear, int actualPeriod);

        // Getters:
        double getTime(){return this->timeInYears;}   // Time in years between initial date and the date it is provided
        double getForward() { return this->forward;}  // Computed in setForward
        // Coupon interest rate (percentage of the nominal that the coupon pays). Variable for floating leg
        double getInterestRate() { return this->zeroCouponInterestRate;}

};

template <class T>
ZeroCoupon<T>::ZeroCoupon(std::tm _date, double _zeroCouponInterestRate, T dayConventionObject, std::tm initialDate)
{
    this-> date = _date;
    this->zeroCouponInterestRate = _zeroCouponInterestRate;
    this->timeInYears = dayConventionObject.compute_daycount(initialDate, _date) / 360;  // dayConventionObject is a Date object (Actual_360 or Thirty_360)
}

template <class T>
void ZeroCoupon<T>::setForward(double timeInYearsBefore, double interestRateBefore, double numOfPeriodsPerYear, int actualPeriod)
{
    // Forward interest rate between last and current period for a given fractional period
    // It is not divided over the difference between last and current period because this difference is 1
    double RF = (this->zeroCouponInterestRate*actualPeriod - interestRateBefore*(actualPeriod-1));
    this->forward = numOfPeriodsPerYear * (exp(RF/numOfPeriodsPerYear) - 1);  // Transform to fractional periods
}

#endif //SQF_ZEROCOUPON_H
