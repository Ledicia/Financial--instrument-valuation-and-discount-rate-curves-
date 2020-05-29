#ifndef SQF_INSTRUMENT_H
#define SQF_INSTRUMENT_H

#include <DiscountFactor/DiscountFactor.h>  // To calculate the interest rate which is not given
#include <vector>

using namespace std;

// Struct (everything is public) from which the specific instruments inherit:
// Although each instrument has different equations to asses them in the present value, all of this equations depend on:
// The interest rate R(t0,ti), the number of days expressed in years b(t0,ti),and the discount factor P(t0,ti)
// Therefore, the instrument class is created with the basic components of the instrument valuations
struct Instrument
{
    double numberOfYearsFromPresentValueToLastPayment;
    vector<DiscountFactor> previousDiscountFactors;  // Vector of previous DF {df0, df1,...df{t-1}}: P(t0,t{i-1})
    double const getNumberOfYearsLastPayment();
    void setNumberOfYearsLastPayment(double lastPaymentYears);  // Keep track of the valuation dates of all the instruments used to build the discount factor curve

    // Getter y setter to complete in each of the classes
    virtual DiscountFactor getDiscountFactor(){};
    virtual void setPreviousDiscountFactors(vector<DiscountFactor> &prevDiscountFactors) {};
};

bool compareEndPeriods(Instrument* a, Instrument* b)
{
    // Returns true if instrument a pays before than instrument b
    // Useful to order the curve build by several instruments by their dates
    return (a->numberOfYearsFromPresentValueToLastPayment < b->numberOfYearsFromPresentValueToLastPayment);
}

double const Instrument::getNumberOfYearsLastPayment()
{
    return this->numberOfYearsFromPresentValueToLastPayment;
}

void Instrument::setNumberOfYearsLastPayment(double lastPaymentYears)
{
    this->numberOfYearsFromPresentValueToLastPayment = lastPaymentYears;
}

#endif //SQF_INSTRUMENT_H
