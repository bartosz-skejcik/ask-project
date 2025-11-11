#ifndef UTILS_H
#define UTILS_H

#include "boost/multiprecision/float128.hpp"
#include <string>

using namespace boost::multiprecision;

using namespace std;

string binary(int n);

string fraction(float128 num);

int exponent(string base);

string ieee74(float128 digit, string bits, int exp, int precision,
              int bias = 16383);

#endif
