/*
 *       ______            __ __                 __
 *      / __/ /_ ____ __  / //_/__ _______  ___ / /
 *     / _// / // /\ \ / / ,< / -_) __/ _ \/ -_) / 
 *    /_/ /_/\_,_//_\_\ /_/|_|\__/_/ /_//_/\__/_/  
 *    
 *   copyright (c) 2021 Gavin Kellam (aka corigan01)
 *   
 *   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 *   to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 *   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                              
 *  
 *   
 */

#include "math.hpp"
#include <System/kout/kout.hpp>

static double math::factorial(double n) {

    if(n == 0) {
        return 1;
    }

    double res = 1;
    for (double i = n; i > 0; i --) {
        res *= i;
    }

    return res;
}


static double math::power(double n, double power) {
    double result = n;

    for(int i = 1; i < power; i++) {
        result = n * result;
    }
    return result;
}

static double math::fmod(double a, double b)
{
    double frac = a / b;
    int floor = frac > 0 ? (int)frac : (int)(frac - 0.9999999999999999);
    return (a - b * floor);
}

double math::sin(double n) {
    n = fmod(n, 2 * PI);
    
    if (n < 0) {
        n = 2 * PI - n;
    }
    
    char sign = 1;
    if (n > PI) {
        n -= PI;
        sign = -1;
    }
    
    double result = n;
    double coefficent = 3.0; // Increment this by 2 each loop

    for(int i = 0; i < 3; i++) { // Change 10 to go out to more/less terms
        double pow = power(n, coefficent);
        double frac = factorial(coefficent);

        // Switch between adding/subtracting
        if(i % 2 == 0) { // If the index of the loop is divided by 2, the index is even, so subtract
            result = result - (pow/frac); // x - ((x^3)/(3!)) - ((x^5)/(5!))...
        } else {
            result = result + (pow/frac); // x - ((x^3)/(3!)) + ((x^5)/(5!))...
        }
        coefficent = coefficent + 2;
    }
    return sign * result;

}

double math::cos(double n) {
    return math::sin(((1.f / 2) * PI) - n);
}