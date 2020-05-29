#ifndef SQF_PUT_H
#define SQF_PUT_H
#include <Instrument/Options/Option.h>

template <class T>
class Put : public Option<T>
{
    public:
        Put(T _dayConventionObject, double _strike, std::tm _presentDate, std::tm _maturityDate, double _annualInterestRate, double _volatility, double _spotValue);
        double compute();
};

template <class T>
Put<T>::Put(T _dayConventionObject, double _strike, std::tm _presentDate, std::tm _maturityDate, double _annualInterestRate, double _volatility, double _spotValue)
{
    this->super(_dayConventionObject, _strike, _presentDate, _maturityDate, _annualInterestRate, _volatility, _spotValue);
}

template <class T>
double Put<T>::compute()
{
    return this->getDiscountFactor() * ( this->N( - this->dminus() ) * this->getStrike() - this->N( - this->dplus() ) * this->getForwardValue() );
}
#endif //SQF_PUT_H
