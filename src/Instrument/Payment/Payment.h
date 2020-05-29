#ifndef SQF_PAYMENT_H
#define SQF_PAYMENT_H

#include <cmath>
#include <iostream>

class Payment
{
    private:
        double nominal;                   // Nominal
        double intRate;                   // Rate of the exponential to calculate discount factor (zero Coupon rate)
        double intYieldCoupon;            // Constant for fix leg, for floating one compute it using zeroCouponCurve
        double numOfYearsFromNow;         // Interval until the final payment
        double numOfYearsFromLastPayment; // Interval from last payment to current moment

    public:
        //Payment(){}
        Payment(double nom,double intR, double intForward, double num, double _numOfYearsFromLastPayment)
        {
            this->nominal = nom;
            this->intRate = intR;
            this->intYieldCoupon = intForward;  // Fix if it is a fixed leg, float if it is a floating one
            this->numOfYearsFromNow= num;
            this->numOfYearsFromLastPayment = _numOfYearsFromLastPayment;
        }

        ~Payment(){}

        // Value of the coupon at the date it is payed (tn)
        double value()
        {
            return (this->nominal * this->intYieldCoupon * this->numOfYearsFromLastPayment);
        }

        // Value of the coupon which pays at period tn in the present date
        double ComputePresentValue()
        {
            // intYieldCoupon will be:
            // Fix payment: the fix interest rate for the period the fix leg pays
            // Float payment: the forward rate obtained from the zero coupon yield curve for the period the float leg pays
            return (this->nominal * this->intYieldCoupon * this->numOfYearsFromLastPayment * exp(-this->intRate * this->numOfYearsFromNow));
        }

        // Getters
        double getForward(){ return this->intYieldCoupon;}
        double getNumOfYearsFromPresentValue(){ return this->numOfYearsFromNow;}
        double getDayCountFromLastPayment(){ return this->numOfYearsFromLastPayment;}
        double getDiscountFactor(){ return exp(-this->intRate * this->numOfYearsFromNow);}

};

#endif //SQF_PAYMENT_H
