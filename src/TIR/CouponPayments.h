#ifndef SQF_COUPONPAYMENTS_H
#define SQF_COUPONPAYMENTS_H

// Defines the coupon payments by its amount and the number of years from present value to the date they are payed
struct CouponPayments
{
    CouponPayments(double d, double c): dateInYears{d}, couponAmount{c}{};
    double dateInYears;
    double couponAmount;
};
#endif //SQF_COUPONPAYMENTS_H
