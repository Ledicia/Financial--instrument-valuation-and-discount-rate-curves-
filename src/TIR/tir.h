#ifndef TIR_H
#define TIR_H

//#include <zeroCuponInterestRate/zeroCuponCurve/zeroCouponCurve.h>
#include <cmath>
#include "CouponPayments.h"
#include <vector>


class tir
{
    private:
        std::vector<CouponPayments> payment;  // Vector of payments
        double marketValue;                 //
    public:
        tir();
        ~tir();

        // Add new couponAmount to the bond
        void addCoupon(double numYearsFromPresentDate, double couponAmount);

        void setMarketValue(double value);
        double presentValueMinusMarketValue(double interest);

        double computeDerivative(double point);  // Needed to compute the TIR
        double computeTir(double startPoint, int iterations);
};

tir::tir(){}
tir::~tir() {}

void tir::addCoupon(double numYearsFromPresentDate, double couponAmount)
{
    payment.push_back(CouponPayments(numYearsFromPresentDate, couponAmount));
}

void tir::setMarketValue(double value)
{
    marketValue = value;
}

double tir::presentValueMinusMarketValue(double interest)
{
    double ret = 0;
    for(int i = 0; i<payment.size(); i++)
    {
        ret = ret + payment[i].couponAmount * exp(-interest * payment[i].dateInYears);
    }
    ret = ret - marketValue;
    return ret;
}

double tir::computeDerivative(double point)
{
    double differential = 0.1;
    return ((presentValueMinusMarketValue(point + differential) - presentValueMinusMarketValue(point)) / differential);
}

double tir::computeTir(double startPoint, int iterations)
{
    double x = startPoint;
    for(int i = 0; i<iterations; i++)
    {
        x = x - (presentValueMinusMarketValue(x) / computeDerivative(x));
    }
    return x;
}

#endif //TIR_H
