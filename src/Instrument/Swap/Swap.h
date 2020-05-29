#ifndef SWAP_H
#define SWAP_H

#include <vector>
#include <Date/Actual_360.h>
#include <Date/Thirty_360.h>
#include <Instrument/Instrument.h>
#include <Instrument/Payment/Payment.h>
#include <ZeroCouponYieldCurve/ZeroCouponYieldCurve.h>

template <class T>
class Swap : public Instrument
{
    private:
        // SWAP VALUATION //
        T zeroCoupon;                          // Object of type zeroCoupon
        double nominal;                        // Nominal
        std::tm presentValueDate;              // Valuation date
        std::tm lastPaymentDate;               // Date of the last payment occurrence
        std::vector<Payment> FixPayment;       // Fix Leg
        std::vector<Payment> VariablePayment;  // Float Leg
        // SWAP DISCOUNT FACTOR //
        double swapFixInterestRate;            // Interest rate between present date and last payment date S(t0,tn)
    public:
        // SWAP VALUATION //
        Swap(double _nominal, T& _zeroCoupon, std::tm lastPayment);
        Swap(double _nominal, T& _zeroCoupon, vector<std::tm> _paymentCalendar, double fixInterestRate);
        ~Swap();

        double computePresentValue();
        void floatPaymentValuations(double numOfPaymentsPerYear);
        void fixPaymentValuations(double interest, double numOfPaymentsPerYear);

        // Getters
        double getVariablePaymentValue(int i){ return this->VariablePayment[i].value();}
        double getFixPaymentValue(int i){ return this->FixPayment[i].value();}

        double getVariableForward(int i){ return VariablePayment[i].getForward();}
        double getDayCountFromLastPayment(int i){ return VariablePayment[i].getDayCountFromLastPayment();}
        double getDiscountFactor(int i){ return VariablePayment[i].getDiscountFactor();}


        // SWAP DISCOUNT FACTOR //
        Swap(double fixIntRate , double numOfMonth);
        Swap(T dayCount, double fixIntRate , std::tm startDate, std::tm endDate);

        // Add the P(t0,ti) such that i = 1,2,...,n to the previousDiscountFactors vector
        // Swaps have several payment dates, and all the DF P(t0,ti) are needed so as to compute P(t0,tn)
        void setPreviousDiscountFactors(vector<DiscountFactor> &prevDiscountFactors);
        DiscountFactor getDiscountFactor();  // P(t0,tn)

};

// SWAP VALUATION //
template <class T>
Swap<T>::Swap(double _nominal, T& _zeroCoupon, std::tm _lastPayment)
{
    this->nominal= _nominal;
    this->zeroCoupon = _zeroCoupon;
    this->presentValueDate = _zeroCoupon.getPresentValue();
    this->lastPaymentDate = _lastPayment;
}

template <class T>
Swap<T>::Swap(double _nominal, T& _zeroCoupon, vector<std::tm> _paymentCalendar, double fixInterestRate)
{
    // Compute payments from a date vector which contains the payment dates (_paymentCalendar)
    this->nominal= _nominal;
    this->zeroCoupon = _zeroCoupon;  // ZeroCouponCurve
    this->presentValueDate = _zeroCoupon.getPresentValue();
    this->lastPaymentDate = _paymentCalendar.back();

    // Initialize date variables: Delta(t) = dateInYears - lastDateInYears
    double dateInYears = 0;
    double lastDateInYears = 0;

    // Add payment object to FixPayment vector (this object has the methods of the Payment class)
    for(int i = 0; i<_paymentCalendar.size(); ++i)
    {
        // Update dates when the payments occur
        // getTimeInYearsFromPresentDate: Diff in years from paymentCalendar[i] to initialDate (class attribute of zeroCouponYieldCurve which represents the present date)
        // getInterpolatedZCRate: interest rate from yield curve for the period in years by interpolating methods
        // getForward(i): computes the forward interest rate between lastDateInYears and dateInYears
        lastDateInYears = dateInYears;
        dateInYears = zeroCoupon.getTimeInYearsFromPresentDate(_paymentCalendar[i]);
        FixPayment.push_back(Payment(this->nominal, this->zeroCoupon.getInterpolatedZCRate(dateInYears), fixInterestRate, dateInYears, dateInYears - lastDateInYears));
        VariablePayment.push_back(Payment(this->nominal, this->zeroCoupon.getInterpolatedZCRate(dateInYears), this->zeroCoupon.getForward(i), dateInYears, dateInYears - lastDateInYears ));
    }
}

template <class T>
double Swap<T>::computePresentValue()
{
    // First compute the fix leg (first loop or sumation), and then subtract the variable payments
    double ret = 0;
    for(int i=0;i<FixPayment.size();i++)
    {
        ret = ret+FixPayment[i].ComputePresentValue();
    }
    for(int i=0;i<FixPayment.size();i++)
    {
        ret = ret-VariablePayment[i].ComputePresentValue();
    }
    return ret;
}

template <class T>
void Swap<T>::fixPaymentValuations(double interest, double numOfPaymentsPerYear)
{
    // Compute fractional payments for fix leg (look at bond implementation)
    std::tm date;
    int lastPayment = (int)round(zeroCoupon.getTimeInDayCountConvention(this->lastPaymentDate));
    double fracNumPaymentsPerYear = 1/(numOfPaymentsPerYear);

    double dateInYears = 0;
    double lastDateInYears = 0;
    cout<<"Payment calendar for the swap fix leg: "<<endl;
    for(double i=fracNumPaymentsPerYear; i <=lastPayment; i=i+fracNumPaymentsPerYear)
    {
        lastDateInYears = dateInYears;
        date = zeroCoupon.getDayCountConvention().generate_tm(this->presentValueDate, i);
        cout<<"dd/mm/yyyy: "<<date.tm_mday<<"/"<<date.tm_mon+1<<"/"<<date.tm_year+1900<<endl;
        FixPayment.push_back(Payment(this->nominal, this->zeroCoupon.getInterpolatedZCRate(i), interest, dateInYears, fracNumPaymentsPerYear )); // Definición de un pago
    }
    cout<<"\n"<<endl;
}

template <class T>
void Swap<T>::floatPaymentValuations(double numOfPaymentsPerYear)
{
    // Compute fractional payments for float leg (get the forward interest rate from the zero coupon curve)
    std::tm date;
    int lastPayment = (int)round(zeroCoupon.getTimeInDayCountConvention(this->lastPaymentDate));
    double fracNumPaymentsPerYear = 1/(numOfPaymentsPerYear);

    double dateInYears = 0;
    double lastDateInYears = 0;
    cout<<"Payment calendar for the swap float leg: "<<endl;
    for(double i=fracNumPaymentsPerYear; i <=lastPayment; i=i+fracNumPaymentsPerYear)
    {
        lastDateInYears = dateInYears;
        date = zeroCoupon.getDayCountConvention().generate_tm(this->presentValueDate, i);
        VariablePayment.push_back(Payment(this->nominal, this->zeroCoupon.getInterpolatedZCRate(i), this->zeroCoupon.getForward(this->presentValueDate, date), dateInYears, fracNumPaymentsPerYear )); // Definición de un pago
    }
    cout<<"\n"<<endl;
}

// SWAP DISCOUNT FACTORS //
template <class T>
Swap<T>::Swap(double fixIntRate, double numOfMonth)
{
    // Constructor given the number of months between present date and last payment date
    this->swapFixInterestRate = fixIntRate;  // S(t0,tn)

    // Compute time in years between the actual date and the date of the last payment
    double lastPaymentInYears = numOfMonth / 12;  // b(t0,tn)

    // Set this time in years (double) in setNumberOfYearsLastPayment (member function of the Instrument Class)
    // It will be useful to order instruments by their finalization date
    this->setNumberOfYearsLastPayment(lastPaymentInYears);
}

template <class T>
Swap<T>::Swap(T dayCount, double fixIntRate, std::tm startDate, std::tm endDate)
{
    // Constructor given the starting date and the end date of the swap payments
    this->swapFixInterestRate = fixIntRate;  // S(t0,tn)

    // Compute time in years between the actual date and the date of the last payment
    double lastPaymentInYears = dayCount.compute_daycount(startDate, endDate)/360;  // b(t0,tn)

    // Set this time in years (double) in setNumberOfYearsLastPayment (member function of the Instrument Class)
    // It will be useful to order instruments by their finalization date
    this->setNumberOfYearsLastPayment(lastPaymentInYears);
}

template <class T>
void Swap<T>::setPreviousDiscountFactors(vector<DiscountFactor> &prevDiscountFactors)
{
    // previousDiscountFactors is an attribute (a vector) of the class from which this class inherits
    this->previousDiscountFactors = prevDiscountFactors;
}

template <class T>
DiscountFactor Swap<T>::getDiscountFactor()
{
    // Compute discount factor (EQUATION 3.6)
    double numberOfYearsBetweenPayments = 0;
    double sum = 0;  // Accumulator for the summation in EQ 3.6

    // Create and initialize the attributes of the discount factor objects to zero
    DiscountFactor actualDF = DiscountFactor();
    DiscountFactor previousDF = DiscountFactor();

    // Before calling this functions, all the discount factors need to be known so as to iterate over them
    for(int i = 0; i<this->previousDiscountFactors.size(); ++i)
    {
        actualDF = previousDiscountFactors[i];  //Get actual discount factor to work with it
        sum = sum + this->swapFixInterestRate * (actualDF.getYearsFromPresentValue() - previousDF.getYearsFromPresentValue()) * actualDF.getDiscountFactor();
        previousDF = actualDF;
    }
    sum = (1 - sum)/(1 + this->swapFixInterestRate * (this->getNumberOfYearsLastPayment() - actualDF.getYearsFromPresentValue()));

    return DiscountFactor(this->getNumberOfYearsLastPayment(), sum);
}

template <class T>
Swap<T>::~Swap(){}
#endif //SWAP_H
