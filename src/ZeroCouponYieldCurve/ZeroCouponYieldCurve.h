#ifndef SQF_ZEROCOUPONYIELDCURVE_H
#define SQF_ZEROCOUPONYIELDCURVE_H

#include <Spline/spline.h>
#include <ZeroCoupon/ZeroCoupon.h>
#include <string>
using namespace std;

template <class T>
class ZeroCouponYieldCurve
{
    private:
        T dayCountConvention;        // Date package object (actual_360 or Thirty_360)
        std::tm initialDate;         // Date where the curve starts (matches valuation date)
        double numOfPeriodsPerYear;  // Define fractional payments (num payments in a year)
        tk::spline spline;           // Interpolate method to extract zeroCoupon rates from not defined periods
        std::vector<ZeroCoupon<T>> zeroCouponVector;  // Vector of zeroCoupon objects (each zeroCoupon is associated to a date)
    public:
        ZeroCouponYieldCurve();
        ZeroCouponYieldCurve (T dayConventionObject, std::tm _initialDate); // dayConvention: Actual_360 or Thirty_360
        
        void addZeroCouponRate(std::tm _date, double _zeroCouponInterestRate);
        void computeZeroCurve();                    // Build the zero coupon yield curve
        double getInterpolatedZCRate(double years); // Get zero coupon rate using interpolation method and the curve
        double getDiscountFactor(int i);

        double getForward(int i);  // Get forwards between the periods used to build the curve
        double getForward(std::tm _firstPeriodDate, std::tm _lastPeriodDate);  // Get forwards between 2 dates

        void setNumOfPeriodsPerYear(double i);

        // Get information about dates
        std::tm getPresentValue();
        T getDayCountConvention();
        double getNumOfPeriodsPerYear();
        double getTimeInYearsFromPresentDate(std::tm _time);
};

template <class T>
ZeroCouponYieldCurve<T>::ZeroCouponYieldCurve() {}

template <class T>
ZeroCouponYieldCurve<T>::ZeroCouponYieldCurve (T dayConventionObject, std::tm _initialDate)
{
    this->dayCountConvention = dayConventionObject;
    this->initialDate = _initialDate;
}

template <class T>
void ZeroCouponYieldCurve<T>::addZeroCouponRate(std::tm _date, double _zeroCouponInterestRate)
{
    // Add zero coupon object to the vector
    // Each zeroCoupon object has a _zeroCouponInterestRate associated to a _date (internal attributes)
    this->zeroCouponVector.push_back(ZeroCoupon<T>( _date,  _zeroCouponInterestRate,
                                        this->dayCountConvention, this->initialDate ));
}

template <class T>
void ZeroCouponYieldCurve<T>::computeZeroCurve()
{
    // Create a zero coupon curve to interpolate the rates that are not in the tables (Rate vs Maturity in years)
    std::vector<double> time;            // Vector of maturities in years
    std::vector<double> zeroCouponRate;  // Vector of zero coupon rates for different maturities

    // zeroCouponVector[i].getTime(): difference of time in years between this->initial_date and _date
    // zeroCouponVector[1].getTime() - zeroCouponVector[0].getTime() = _date[1] - _date[0] (in years)
    // The inverse of two consecutive date payments is the number of periods a year is divided in
    this->numOfPeriodsPerYear = (double)round(1/(this->zeroCouponVector[1].getTime() - this->zeroCouponVector[0].getTime()));

    for(int i = 0; i<this->zeroCouponVector.size(); ++i)
    {
        if(i == 0)
        {
            // First element of the rate vector (Forward rate from start period (i=0) to following one (i+1))
            // zeroCouponVector[i].getTime(): diff in years from initial_period to _date[i]
            this->zeroCouponVector[i].setForward( 0, 0, this->numOfPeriodsPerYear, i+1);
            time.push_back(this->zeroCouponVector[i].getTime());
            zeroCouponRate.push_back(this->zeroCouponVector[i].getInterestRate());
        }
        else
        {
            // Elements of forward rate vector but 1st one (forward rate between two consecutive dates)
            // zeroCouponVector[i-1].getTime(): diff in years from initial_period to _date[i-1] (_date[i-1] end of the (i-1) period and start of the following one (i))
            // zeroCouponVector[i-1].getInterestRate(): interest rate from the previous period (period from i-1 to i)
            // actualPeriod = i+1 (difference of periods (in years) from 0 (initialDate) to the actual one)
            // The ith element of the zeroCoupon forward rate is the forward rate from i to i+1 and is set in the
            // ith zeroCoupon object of the zeroCouponVector. IT can be access by getFoward(int i)
            this->zeroCouponVector[i].setForward(this->zeroCouponVector[i-1].getTime(),
                    this->zeroCouponVector[i-1].getInterestRate(), this->numOfPeriodsPerYear, i+1);

            // Build the curve
            time.push_back(this->zeroCouponVector[i].getTime());
            zeroCouponRate.push_back(this->zeroCouponVector[i].getInterestRate());  // _zeroCouponInterestRate
        }
    }
    this->spline.set_points(time,zeroCouponRate);  // Prints the interest rates for diff periods
}

template <class T>
double ZeroCouponYieldCurve<T>::getForward(int i)
{
    // Forward rate of the zeroCoupon[i] object from period i to period i+1
    return this->zeroCouponVector[i].getForward();
}

template <class T>
double ZeroCouponYieldCurve<T>::getInterpolatedZCRate(double years)
{
    // Returns the interpolation: forward rate for the period of length years (date: initial_date + years)
    return this->spline(years);
}

template <class T>
double ZeroCouponYieldCurve<T>::getForward(std::tm _firstPeriodDate, std::tm _lastPeriodDate)
{
    // Forward rate between _firstPeriodDate and _lastPeriodDate
    double _firstDate = this->dayCountConvention.compute_daycount(this->initialDate, _firstPeriodDate) / 360; // In years
    double _lastDate = this->dayCountConvention.compute_daycount(this->initialDate, _lastPeriodDate) / 360;   // In years
    double _numOfPeriodsPerYear = (double)round(1/(_lastDate - _firstDate));

    // Forward rate from _firstDate to _lastDate. Here fractional periods are consider since
    // multiplying (ZC_last*_lastDate-ZC_first*_firstDate) by _numOfPeriodsPerYear is the same as computing
    // (ZC_last*_lastDate-ZC_first*_firstDate)/(_lastDate-_firstDate), so diff in dates is considered (as an approx)
    double RF = (this->getInterpolatedZCRate(_lastDate)*(_lastDate * _numOfPeriodsPerYear) -
                this->getInterpolatedZCRate(_firstDate)*(_firstDate * _numOfPeriodsPerYear));
    // Switching from continuously compounded to annually compounded
    // Divided by _numOfPeriodsPerYear cause it cancels with this factor in RF,
    // so it is the forward interest in the units of _numOfPeriodsPerYear (years)
    return _numOfPeriodsPerYear * (exp(RF/_numOfPeriodsPerYear) - 1);
}

template <class T>
void ZeroCouponYieldCurve<T>::setNumOfPeriodsPerYear(double i)
{
    this->numOfPeriodsPerYear = i;
}

template <class T>
double ZeroCouponYieldCurve<T>::getDiscountFactor(int i)
{
    return exp(- this->zeroCouponVector[i].getInterestRate()* this->zeroCouponVector[i].getTime());
}

template <class T>
std::tm ZeroCouponYieldCurve<T>::getPresentValue()
{
    return this->initialDate;
}

template <class T>
T ZeroCouponYieldCurve<T>::getDayCountConvention()
{
    return this->dayCountConvention;
}

template <class T>
double ZeroCouponYieldCurve<T>::getNumOfPeriodsPerYear()
{
    return this->numOfPeriodsPerYear;
}

template <class T>
double ZeroCouponYieldCurve<T>::getTimeInYearsFromPresentDate(std::tm _time)
{
    return this->dayCountConvention.compute_daycount(this->initialDate, _time) / 360;  // In year units (days/360)
}

#endif //SQF_ZEROCOUPONYIELDCURVE_H
