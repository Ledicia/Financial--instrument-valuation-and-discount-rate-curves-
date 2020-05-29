#ifndef SQF_DISCOUNTFACTORBOOTSTRAP_H
#define SQF_DISCOUNTFACTORBOOTSTRAP_H

#include <DiscountFactor/DiscountFactor.h>
#include <spline/spline.h>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

// Define a namespace to group the functions related to the discount factor calculation
// Instead of defining a map, a spline method was downloaded from the internet to set the points of the curve
// and calculate the interpolation, which is the interest rate for a period (t0,tk) where tk is not a calibrated date
namespace DiscountFactorBootstrap
{
        vector<double> discountFactorVect;  // Vector to store the discount factors
        vector<double> discountFactorTime;  // Vector to store the dates
        tk::spline spline;                  // To interpolate the rates that are not given
        int increment;                      // Keep track of how many points there are in the discount factor curve

        // Add data to the discount curve
        void addPoint(double time, double discountFactor)
        {
            increment = increment +1;  // Number of points the curve has

            // Add elements to the curve
            discountFactorVect.push_back(discountFactor);
            discountFactorTime.push_back(time);

            // To build the discount factor curve at least 3 points are needed (if not we just have a point or a line)
            if(increment > 2)
            {
                tk::spline auxSpline;
                auxSpline.set_points(discountFactorTime, discountFactorVect);

                // Switch pointers so spline is auxSpline
                std::swap(auxSpline, spline);
            }
        };

        // Return the interpolated discount factor once the curve has been build
        double getInterpolatedDiscountFactor(double timeInYears)
        {
            return spline(timeInYears);
        };

        // Get total number of discount factors introduced in the spline
        int getIncrement()
        {
            return increment;
        }
};
#endif //SQF_DISCOUNTFACTORBOOTSTRAP_H
