#ifndef SQF_CALL_H
#define SQF_CALL_H

#include <Instrument/Options/Option.h>

template <class T>
class Call : public Option<T>
{
    public:
        Call(T _dayConventionObject, double _strike, std::tm _presentDate, std::tm _maturityDate,
             double _annualInterestRate, double _volatility, double _spotValue);
        double compute();
};

// Cosntructor
template <class T>
Call<T>::Call(T _dayConventionObject, double _strike, std::tm _presentDate, std::tm _maturityDate, double _annualInterestRate, double _volatility, double _spotValue)
{
     this->super(_dayConventionObject, _strike, _presentDate, _maturityDate, _annualInterestRate, _volatility, _spotValue);
}

template <class T>
double Call<T>::compute()
{
    return this->getDiscountFactor() * ( this->N( this->dplus() ) * this->getForwardValue()
                                                    - this->N( this->dminus() ) * this->getStrike() );
}

#endif //SQF_CALL_H
