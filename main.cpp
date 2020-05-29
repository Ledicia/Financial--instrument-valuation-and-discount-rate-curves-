#include <TIR/tir.h>
#include <Date/Actual_360.h>
#include <ZeroCouponYieldCurve/ZeroCouponYieldCurve.h>
#include <Instrument/Swap/Swap.h>
#include <Instrument/Deposit/Deposit.h>
#include <cmath>
#include <Instrument/Options/Option.h>
#include <Instrument/Options/Call/Call.h>
#include <Instrument/Options/Put/Put.h>

using namespace std;

void testValuations(){

    // Test TIR
    tir TIR = tir();  // Create tir object

    TIR.setMarketValue(98.39);
    TIR.addCoupon(0.5, 3);
    TIR.addCoupon(1, 3);
    TIR.addCoupon(1.5, 3);
    TIR.addCoupon(2, 103);

    if (abs(0.06759 - TIR.computeTir(0, 1000)) <= 1e-5){
        std::cout << "TIR calculation test okay" << endl;
    }

    // Create objects to compute swap and then test it
    Actual_360 actual360 = Actual_360();
    std::tm presentDate = actual360.make_tm(2016, 04, 01);
    ZeroCouponYieldCurve<Actual_360> zeroCouponCurve = ZeroCouponYieldCurve<Actual_360>(actual360, presentDate);
    std::vector<std::tm> paymentDates;

    paymentDates.push_back(actual360.make_tm(2016, 10, 03));
    paymentDates.push_back(actual360.make_tm(2017, 04, 03));
    paymentDates.push_back(actual360.make_tm(2017, 10, 02));
    paymentDates.push_back(actual360.make_tm(2018, 04, 02));

    std::vector<double> interestRate;

    interestRate.push_back(0.0474);
    interestRate.push_back(0.0500);
    interestRate.push_back(0.0510);
    interestRate.push_back(0.0520);

    for( int i = 0; i < interestRate.size(); ++i)
    {
        zeroCouponCurve.addZeroCouponRate(paymentDates[i], interestRate[i]);
    }
    zeroCouponCurve.computeZeroCurve();

    double nominal = 100000000;
    double semiAnnualFixInterestRate = 0.05;

    Swap<ZeroCouponYieldCurve<Actual_360>> swap=Swap<ZeroCouponYieldCurve<Actual_360>>(nominal, zeroCouponCurve, paymentDates, semiAnnualFixInterestRate);

    // Test forward rates obtained from zero coupon
    if (abs(swap.getVariableForward(0) - 0.04797) <= 1e-5){
        std::cout << "Swap forward rate for first payment test okay " << endl;
    }

    if (abs(swap.getVariableForward(1) - 0.05330) <= 1e-5){
        std::cout << "Swap forward rate for second payment test okay" << endl;
    }

    if (abs(swap.getVariableForward(2) - 0.05371) <= 1e-5){
        std::cout << "Swap forward rate for third payment test okay" << endl;
    }

    if (abs(swap.getVariableForward(3) - 0.05576) <= 1e-5){
        std::cout << "Swap forward rate for last payment test okay" << endl;
    }

    // Test day count
    if (abs(swap.getDayCountFromLastPayment(0) - 0.514) <= 1e-3){
        std::cout << "Swap day count in years for first payment test okay " << endl;
    }

    if (abs(swap.getDayCountFromLastPayment(1) - 0.506) <= 1e-3){
        std::cout << "Swap day count in years for second payment test okay " << endl;
    }

    if (abs(swap.getDayCountFromLastPayment(2) - 0.506) <= 1e-3){
        std::cout << "Swap day count in years for third payment test okay " << endl;
    }

    if (abs(swap.getDayCountFromLastPayment(3) - 0.506) <= 1e-3){
        std::cout << "Swap day count in years for last payment test okay " << endl;
    }

    // Test discount factor
    if (abs(swap.getDiscountFactor(0) - 0.9759) <= 1e-4){
        std::cout << "Swap discount factor for first payment test okay " << endl;
    }

    if (abs(swap.getDiscountFactor(1) - 0.9503) <= 1e-4){
        std::cout << "Swap discount factor for second payment test okay " << endl;
    }

    if (abs(swap.getDiscountFactor(2) - 0.9251) <= 1e-4){
        std::cout << "Swap discount factor for third payment test okay " << endl;
    }

    if (abs(swap.getDiscountFactor(3) - 0.8997) <= 1e-4){
        std::cout << "Swap discount factor for last payment test okay " << endl;
    }

    // Test Variable payment value
    if (abs(swap.getVariablePaymentValue(0) - 2465460.300) <= 1e3){
        std::cout << "Swap first float payment test okay " << endl;
    }
    else{
        std::cout << "Error percentage: " << (swap.getVariablePaymentValue(0) - 2465460.300)/2465460.300 << endl;
    }

    if (abs(swap.getVariablePaymentValue(1) - 2696868.370) <= 1e4){
        std::cout << "Swap second float payment test okay " << endl;
    }
    else{
        std::cout << "Error percentage: " << (swap.getVariablePaymentValue(0) - 2696868.370)/2696868.370 << endl;
    }

    if (abs(swap.getVariablePaymentValue(2) - 2717649.820) <= 1e4){
        std::cout << "Swap third float payment test okay " << endl;
    }
    else{
        std::cout << "Error percentage: " << (swap.getVariablePaymentValue(0) - 2717649.820)/2717649.820 << endl;
    }

    if (abs(swap.getVariablePaymentValue(3) - 2821619.450) <= 1e4){
        std::cout << "Swap last float payment test okay " << endl;
    }
    else{
        std::cout << "Error percentage: " << (swap.getVariablePaymentValue(0) - 2821619.450)/2821619.450 << endl;
    }

    // Test Swap present value
    if (abs(swap.computePresentValue() + 492525.424) <= 1e3){
        std::cout << "Swap valuation test okay " << endl;
    }
    else{
        std::cout << "Error percentage: " << (swap.computePresentValue() + 492525.424)/492525.424 << endl;
    }
}

void testDiscountFactors(){

    // Vector of pointers to store the memory address of the specific instruments
    std::vector<Instrument*> instrumentVector;
    std::vector<DiscountFactor> discFact;

    Deposit<Actual_360>* deposit = new Deposit<Actual_360>(0.05, 6);
    instrumentVector.push_back(deposit);
    // Create a new discount factor object
    DiscountFactor discountFactor = deposit->getDiscountFactor();
    discFact.push_back(discountFactor);

    // Test
    if (abs(discountFactor.getDiscountFactor() - 0.9756097561) <= 1e-4){
        std::cout << "6month deposit discount factor okay " << endl;
    }
    else{
        std::cout << "6month deposit discount factor error. The actual value is: " << discountFactor.getDiscountFactor() << endl;
    }

    // Create and allocate the object and pass its address to a raw pointer
    Swap<Actual_360>* swap = new Swap<Actual_360>(0.055, 12);
    // Access to the encapsulated pointer: same as (*swap).setPreviousDiscountFactors(discFact)
    swap->setPreviousDiscountFactors(discFact);
    instrumentVector.push_back(swap);

    discountFactor = swap->getDiscountFactor();
    discFact.push_back(discountFactor);

    // Test
    if (abs(discountFactor.getDiscountFactor() - 0.9478672986) <= 1e-3){
        std::cout << "12month swap discount factor okay " << endl;
    }
    else{
        std::cout << "12month swap discount factor error. The actual value is: " << discountFactor.getDiscountFactor() << endl;
    }

    swap = new Swap<Actual_360>(0.06, 18);
    swap->setPreviousDiscountFactors(discFact);
    instrumentVector.push_back(swap);

    discountFactor = swap->getDiscountFactor();
    discFact.push_back(discountFactor);

    // Test
    if (abs(discountFactor.getDiscountFactor() - 0.915658215615534) <= 1e-2){
        std::cout << "18month swap discount factor okay " << endl;
    }
    else{
        std::cout << "18month discount factor error. The actual value is: " << discountFactor.getDiscountFactor()<< endl;
    }

    swap = new Swap<Actual_360>(0.064, 24);
    swap->setPreviousDiscountFactors(discFact);
    instrumentVector.push_back(swap);

    discountFactor = swap->getDiscountFactor();
    discFact.push_back(discountFactor);

    // Test
    if (abs(discountFactor.getDiscountFactor() - 0.8818172771219989) <= 1e-3){
        std::cout << "24month swap discount factor okay" << endl;
    }
    else{
        std::cout << "24month swap discount factor error. The actual value is: " << discountFactor.getDiscountFactor() << endl;
    }
}

void buildDiscountFactorCurve(){

    // Vector of pointers to instrument object memory addresses
    // Each object will be an instrument used to finance the financial institution (build discount factor curve)
    std::vector<Instrument *> instrumentVector;

    // Vector of discountFactor objects:Stores the discount factors of each instrument
    std::vector<DiscountFactor> discountFactor;

    // Unordered instruments
    // Create the instruments needed to build the discount factor curve and save them in the instrumentVector
    instrumentVector.push_back(new Swap<Actual_360>(0.064, 24));
    instrumentVector.push_back(new Deposit<Actual_360>(0.05, 6));
    instrumentVector.push_back(new Swap<Actual_360>(0.06, 18));
    instrumentVector.push_back(new Swap<Actual_360>(0.055, 12));

    // Sort the instruments by last payment date so as to build the curve
    // Use the bool function defined in Instrument Class
    std::sort(instrumentVector.begin(), instrumentVector.end(), compareEndPeriods);

    // Print discount factors
    cout << "Discount Factor: " << endl;
    for (int i = 0; i < instrumentVector.size(); ++i) {
        // Set the discount factor vector
        // The Swap will need them, so it has a function defined that inherits from the one defined in Instrument
        // The deposit wont need them, so it has no function implemented
        instrumentVector[i]->setPreviousDiscountFactors(discountFactor);

        // Get the discount factors. Specific function for each object in instrumentVector
        // The swap needs the previous discount factors so as to compute the one at period n
        discountFactor.push_back(instrumentVector[i]->getDiscountFactor());

        // Build the discount factor curve
        DiscountFactorBootstrap::addPoint(instrumentVector[i]->getDiscountFactor().getYearsFromPresentValue(),
                                          instrumentVector[i]->getDiscountFactor().getDiscountFactor());

        // DiscountFactor object has overloaded operator <<
        cout << discountFactor[i] << endl;
    }

}

void testsPractice3(){
    // Date convenction
    Actual_360 actual360 = Actual_360();

    std::tm valueDate =actual360.make_tm(2016, 1, 1);
    std::tm maturityDate = actual360.make_tm(2017, 1, 1);
    double volatility = 0.25;
    double interestRate = 0.05;
    double strike = 100;
    double spot = 102;

    Call<Actual_360> call = Call<Actual_360>(actual360, strike, valueDate, maturityDate, interestRate, volatility, spot);

    // Test call option
    if (abs(call.compute() - 20.0369) <= 1e-2){
        std::cout << "Calculated value of the call option is within tolerance" << endl;
    }
    else{
        std::cout << "Error in the calculated value of the call option " << endl;
    }


    Put<Actual_360> put = Put<Actual_360>(actual360, strike, valueDate, maturityDate, interestRate, volatility, spot);

    // Test put option
    if (abs(put.compute() - 8.5082) <= 1e-2){
        std::cout << "Calculated value of the put option is within tolerance" << endl;
    }
    else{
        std::cout << "Error in the calculated value of the put option " << endl;
    }

}

void practice3()
{

    Actual_360 actual360 = Actual_360();

    std::tm valueDate = actual360.make_tm(2011, 1, 1);
    std::tm maturityDate = actual360.make_tm(2012, 1, 20);

    Call<Actual_360> call = Call<Actual_360>(actual360, 10, valueDate, maturityDate, 0.05, 0.25, 102 );
    Put<Actual_360> put = Put<Actual_360>(actual360, 10, valueDate, maturityDate, 0.05, 0.25, 102 );

    cout << "Call " << call.compute() << endl;
    cout << "Put " << put.compute() << endl;

    cout << "D minus " << call.dminus() << endl;
    cout << "D plus " << call.dplus() << endl;

}

int main() {

    cout<<"\n----------------------------------------------"<<endl;
    cout<<"Practice 1: Present Date Valuations "<<endl;
    cout<<"----------------------------------------------\n"<<endl;
    testValuations();

    cout<<"\n"<<endl;
    cout<<"\n----------------------------------------------"<<endl;
    cout<<"Practice 2: Discount Factors "<<endl;
    cout<<"----------------------------------------------\n"<<endl;
    testDiscountFactors();

    cout<<"\n"<<endl;
    cout<<"\n----------------------------------------------"<<endl;
    cout<<"Practice 2: Discount Factor Curve "<<endl;
    cout<<"----------------------------------------------\n"<<endl;
    buildDiscountFactorCurve();

    cout<<"\n"<<endl;
    cout<<"\n----------------------------------------------"<<endl;
    cout<<"Practice 3: Option Valuation "<<endl;
    cout<<"----------------------------------------------\n"<<endl;
    practice3();

    return 0;
}