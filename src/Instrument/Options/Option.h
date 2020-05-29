#ifndef SQF_OPTION_H
#define SQF_OPTION_H

#include <cmath>
#include <ctime>
#include <random>
#include <iostream>

using namespace std;

// Since the equations to valuate options are the same, this class implements them so as specific classes for the
// different options can inherit from here
template <class T>
class Option
{
    private:
        T dayConventionObject;      // Compute number of days between two dates
        double strike;              // Price at which the option will be executed in the future (K)
        double maturity;            // Time at which the option can be executed (Tau)
        double annualInterestRate;  // r
        double volatility;          // Sigma
        double spotValue;           // Current value of the asset (S)
        double discountFactor;      // Discount factor to calculate the payment at current date (D)
    public:
        Option(){};  // Default constructor

        // Since the call and put options have the same parameters, the constructor is defined in here (super)
        // and in the specific class it will be called inside its constructor so as to not copy this lines again
        void super(T _dayConventionObject, double _strike, std::tm _presentDate,
                   std::tm _maturityDate, double _annualInterestRate, double _volatility, double _spotValue);

        // Defined in each of the classes (each one has an specific equation although with the same parameters)
        virtual double compute() {return 0;}  // For inheriting (in the proper classes it will return another value)

        // Functions that compute the variables needed in the Black-Scholes equation
        double dplus();
        double dminus();
        double N(double);  // Cumulative probability

        // Getters
        double getStrike(){ return this->strike;};
        double getTimeUntilMaturity(){ return this->maturity;};
        double getAnnualIntRate(){ return this->annualInterestRate;};
        double getVolatility(){ return this->volatility;};
        double getSpotValue(){ return this->spotValue;};
        double getDiscountFactor(){ return this->discountFactor;};
        double getForwardValue(){return ( this->getSpotValue() / this->getDiscountFactor() );}; // Forward value of the underlying asset

};

// Class constructor
template <class T>
void Option<T>::super(T _dayConventionObject, double _strike, std::tm _presentDate, std::tm _maturityDate,
                      double _annualInterestRate, double _volatility, double _spotValue)
{
    this->dayConventionObject = _dayConventionObject;
    this->strike = _strike;
    this->maturity = this->dayConventionObject.compute_daycount(_presentDate, _maturityDate)/360;  // Diff in years
    this->annualInterestRate = _annualInterestRate;
    this->volatility = _volatility;
    this->spotValue = _spotValue;

    this->discountFactor = exp(- this->getAnnualIntRate() * this->getTimeUntilMaturity());
}

template <class T>
double Option<T>::dplus()
{
    double firstTerm = ( 1 / ( this->getVolatility() * sqrt(this->getTimeUntilMaturity() ) ) );
    double secondTerm = log(this->getSpotValue() / (this->getDiscountFactor() * this->getStrike()) );
    double lastTerm = 0.5 * this->getVolatility() * this->getVolatility() * this->getTimeUntilMaturity();

    return ( firstTerm * (secondTerm + lastTerm));
}

template <class T>
double Option<T>::dminus()
{
    double firstTerm = ( 1 / ( this->getVolatility() * sqrt(this->getTimeUntilMaturity() ) ) );
    double secondTerm = log(this->getSpotValue() / (this->getDiscountFactor() * this->getStrike()) );
    double lastTerm = 0.5 * this->getVolatility() * this->getVolatility() * this->getTimeUntilMaturity();

    return ( firstTerm * (secondTerm - lastTerm));
}

template <class T>
double Option<T>::N(double d)// N(d) = Phi(-infinite, d)
{
    // The gaussian error function is related to the gaussian cumulative probability:
    return std::erfc(-d/std::sqrt(2))/2;
}
#endif //SQF_OPTION_H
