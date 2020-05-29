#ifndef SQF_DEPOSIT_H
#define SQF_DEPOSIT_H

#include <ctime>
#include <Instrument/Instrument.h>

template <class T>
class Deposit : public Instrument
{
    private:
        double interestRate;  // Interes rate of the deposit R(t0,ti)
    public:
        Deposit(T dayCount, double interest, std::tm startDate, std::tm endDate);
        Deposit(double interest, double numOfMonth);
        DiscountFactor getDiscountFactor();  // Returns the discount factor object
};

// Constructor from two dates
template <class T>
Deposit<T>::Deposit(T dayCount, double interest, std::tm startDate, std::tm endDate)
{
    // Compute time in years between the actual date and the date of the last payment
    double lastPaymentInYears = dayCount.compute_daycount(startDate, endDate)/360;

    // Set this time in years (double) in setNumberOfYearsLastPayment (member function of the Instrument Class)
    // It will be useful to order instruments by their finalization date
    this->setNumberOfYearsLastPayment(lastPaymentInYears);

    // Set the fix interest rate
    this->interestRate = interest;
}

// Constructor from the number of months between the start date (when valuating the deposit), and the payment date
template <class T>
Deposit<T>::Deposit(double interest, double numOfMonth)
{
    // Compute time in years between the actual date and the date of the last payment
    double lastPaymentInYears = numOfMonth / 12;

    // Set this time in years (double) in setNumberOfYearsLastPayment (member function of the Instrument Class)
    // It will be useful to order instruments by their finalization date
    this->setNumberOfYearsLastPayment(lastPaymentInYears);

    // Set the fix interest rate
    this->interestRate = interest;
}

template <class T>
DiscountFactor Deposit<T>::getDiscountFactor()
{
    // Compute discount factor of a deposit (EQUATION 3.1)
    double discountFactor = 1 / (1 + this->interestRate * this->getNumberOfYearsLastPayment());
    return DiscountFactor(this->getNumberOfYearsLastPayment(), discountFactor); // Returns a discount factor object
    // this->getNumberOfYearsLastPayment(): returns the number of years from last payment specified and setted
    // in the constructor of a Deposit object
}

#endif //SQF_DEPOSIT_H
