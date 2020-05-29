#ifndef BOND_H
#define BOND_H

#include <Date/Actual_360.h>
#include <Date/Thirty_360.h>
#include <Instrument/Instrument.h>
#include <vector>
#include <Instrument/Payment/Payment.h>

template <class T>
class Bond : public Instrument
{
	private:
        // Variables which define the bond
        double initialCapital;            // Nominal
        std::tm presentValueDate;         // Date of the present value day
        std::tm lastPaymentDate;          // Date of the last payment
        T zeroCoupon;                     // Zero coupon object (allows one to extract forward)
        std::vector<Payment> FixPayment;  // Vector of type Payment (it has its properties implemented)

	public:
        Bond(double _initialCapital, T& _zeroCoupon, std::tm lastPayment);  // Default constructor
        Bond(double _initialCapital, T& _zeroCoupon, std::vector<std::tm> _paymentCalendar, double fixInterestRate);
		~Bond();

        double computePresentValue();
        void fixPaymentValuations(double interest, double numOfPaymentsPerYear);

        // Getter of the vector of payments
        std::vector<Payment> getPaymentVector(){return this->FixPayment;}
};

template <class T>
Bond<T>::Bond(double _initialCapital, T& _zeroCoupon, std::tm lastPayment)
{
    this->initialCapital = _initialCapital;
    this->zeroCoupon = _zeroCoupon;
    this->presentValueDate = _zeroCoupon.getPresentValue();
    this->lastPaymentDate = lastPayment;
}


template <class T>
Bond<T>::Bond(double _initialCapital, T& _zeroCoupon, std::vector<std::tm> _paymentCalendar, double fixInterestRate)
{
    // Compute payments from a date vector which contains the payment dates
    this->initialCapital= _initialCapital;
    this->zeroCoupon = _zeroCoupon;
    this->presentValueDate = _zeroCoupon.getPresentValue();
    this->lastPaymentDate = _paymentCalendar.back();  // Returns a reference to last payment in the vector

    // Initialize date variables: Delta(t) = dateInYears - lastDateInYears
    double dateInYears = 0;
    double lastDateInYears = 0;

    // Add payment object to FixPayment vector (this object has the methods of the Payment class)
    for(int i = 0; i<_paymentCalendar.size(); ++i)
    {
        // Update dates when the payments occur
        // getTimeInDayCountConvention: Diff in years from paymentCalendar[i] to initialDate (class attribute of zeroCouponYieldCurve)
        // getInterpolatedZCRate: interest rate from yield curve for the period in years by interpolating methods
        lastDateInYears = dateInYears;
        dateInYears = zeroCoupon.getTimeInDayCountConvention(_paymentCalendar[i]);
        FixPayment.push_back(Payment(this->initialCapital, this->zeroCoupon.getInterpolatedZCRate(dateInYears),
                fixInterestRate, dateInYears, dateInYears - lastDateInYears));
    }
}

template <class T>
double Bond<T>::computePresentValue()
{
    // Update the value of the bond (sum of all payments). Each payment is calculated through eq 2.7 of notes
    double ret = 0;
    for(int i=0;i<FixPayment.size();i++)
    {
        ret = ret+FixPayment[i].ComputePresentValue();
    }
    return ret;
}

// Compute fractional payments
template <class T>
void Bond<T>::fixPaymentValuations(double interest, double numOfPaymentsPerYear)
{
    std::tm date;
    int lastPayment = (int)round(zeroCoupon.getTimeInDayCountConvention(this->lastPaymentDate));  // Num years from initialDate
    double fracNumPaymentsPerYear = 1/(numOfPaymentsPerYear);

    cout<<"The payment calendar for the Fix Payments will be: "<<endl;
    for(double i=fracNumPaymentsPerYear; i <=lastPayment; i=i+fracNumPaymentsPerYear)
    {
        // DayCountCalculator::generate_tm:
        // adds i to presentValueDate (i is the time in years we want to add, next payment period)
        date = zeroCoupon.getDayCountConvention().generate_tm(this->presentValueDate, i);
        cout<<"dd/mm/yyyy: "<<date.tm_mday<<"/"<<date.tm_mon+1<<"/"<<date.tm_year+1900<<endl;
        FixPayment.push_back(Payment(this->initialCapital, this->zeroCoupon.getInterpolatedZCRate(i), interest,
                i, fracNumPaymentsPerYear )); // Payment definition between a period and the following one
    }
    cout<<"\n"<<endl;
}

template <class T>
Bond<T>::~Bond() {}
#endif
