#ifndef SQF_DISCOUNTFACTOR_H
#define SQF_DISCOUNTFACTOR_H

#include <DiscountFactorBootstrap/DiscountFactorBootstrap.h>

// Class to create discount factor objects:
// It actually does not calculate the discount factor(for that we use DiscountFactorBootstrap namespace)
// It is used to create a discount factor object with the info of the instrument for which it was created
class DiscountFactor
{
private:
    double yearsFromPresentValue;  // Number of years between start and end date
    double discountFactorValue;    // Value of the calculated discount factor
public:
    DiscountFactor();                                                               // Default constructor
    DiscountFactor(double numYearsFromPresentValue, double valueOfDiscountFactor);  // Constructor

    // Getters
    double getDiscountFactor();
    double getYearsFromPresentValue();

    // Overload << operator to get discount factor
    friend ostream& operator<<(ostream& os, const DiscountFactor& dt);
};

// Initialize the constructor
DiscountFactor::DiscountFactor()
{
    this->yearsFromPresentValue = 0;
    this->discountFactorValue = 0;
}

// Build the discount factor object
DiscountFactor::DiscountFactor(double numYearsFromPresentValue, double valueOfDiscountFactor)
{
    this->yearsFromPresentValue = numYearsFromPresentValue;
    this->discountFactorValue = valueOfDiscountFactor;
}

// Get the discount factor of a discount factor object: P(t0,tn)
double DiscountFactor::getDiscountFactor()
{
    return this->discountFactorValue;
}

// Get the number of years between present value and the moment the last payment of a discount factor object: b(t0,tn)
double DiscountFactor::getYearsFromPresentValue()
{
    return this->yearsFromPresentValue;
}

// Print the discount factor for a period from present value to x months later
ostream& operator<<(ostream& os, const DiscountFactor& dt)
{
    // Print discount factor between two periods given in months
    os << "Month from now: "<<dt.yearsFromPresentValue*12<<"\nDiscount factor: "<<dt.discountFactorValue<<"\n";
    return os;
}

#endif //SQF_DISCOUNTFACTOR_H
