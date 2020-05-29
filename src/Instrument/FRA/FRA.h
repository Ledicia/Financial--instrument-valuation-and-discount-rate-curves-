#ifndef SQF_FRA_H
#define SQF_FRA_H

#include <Instrument/Instrument.h>
#include <ctime>

// FRA: Instrument which involves two parts. The payment depends on whether the fixed future rate f(t0,t1,t2) settled
// with the contract is greater or less than the libor interest rate R(t1,t2). This contract is valued in t0 and occurs
// between t1 and t2.
template <class T>
class FRA : public Instrument
{
    private:
        double fraInterestRate;   // FRA rate f(t0,t1,t2) fixed in t0 for period between t1 and t2
        double dayCountFactor;    // Number of days between two dates in years b(t1,t2)
        double startDateInYears;  // Number of days between the present period and the one on which the FRA starts b(t0,t1)
    public:
        FRA(T dayCount, double interest, std::tm presentValue, std::tm startDate, std::tm endDate);
        FRA(double interest, double startDateInYears, double endDateInYears);
        DiscountFactor getDiscountFactor();
};

// Constructor given the dates between the FRA is happening and the present date on which we want to valuate the FRA
template <class T>
FRA<T>::FRA(T dayCount, double interest, std::tm presentValue, std::tm startDate, std::tm endDate)
{
    this->fraInterestRate = interest;
    double lastPaymentInYears = dayCount.compute_daycount(presentValue, endDate)/360;  // b(t0,t2)

    // Set this time in years (double) in setNumberOfYearsLastPayment (member function of the Instrument Class)
    // It will be useful to order instruments by their finalization date
    this->setNumberOfYearsLastPayment(lastPaymentInYears);

    double yearsBetweenStartDateAndEndDate = dayCount.compute_daycount(startDate, endDate)/360; // b(t1,t2)
    this->dayCountFactor = yearsBetweenStartDateAndEndDate;

    double _startDateInYears = dayCount.compute_daycount(presentValue, startDate)/360;  // b(t0,t1)
    this->startDateInYears = _startDateInYears;
}

// Constructor given the number of months between t0 and t1, and the number between t0 and t2
template <class T>
FRA<T>::FRA(double interest, double startDateInMonth, double endDateInMonth)
{
    this->fraInterestRate = interest;

    // Set this time in years (double) in setNumberOfYearsLastPayment (member function of the Instrument Class)
    // It will be useful to order vector<Instrument*> in main.cpp to build discount factor curve
    this->setNumberOfYearsLastPayment(endDateInMonth / 12);           // b(t0,t2)

    this->dayCountFactor = (endDateInMonth - startDateInMonth) / 12;  // b(t1,t2)

    this->startDateInYears = startDateInMonth / 12;                   // b(t0,t1)
}

template <class T>
DiscountFactor FRA<T>::getDiscountFactor()
{
    // Use getInterpolatedDiscountFactor method from DiscountFactorBootstrap to obtain the discount factor from t0 to t1
    double discountFactorStartPeriod = DiscountFactorBootstrap::getInterpolatedDiscountFactor(this->startDateInYears);

    // EQUATION 3.10: Discount factor between t0 and t2
    double discountFactor = discountFactorStartPeriod / (1 + this->fraInterestRate * this->dayCountFactor);
    return DiscountFactor(this->getNumberOfYearsLastPayment(), discountFactor);  // Returns a discount factor object
}

#endif //SQF_FRA_H
