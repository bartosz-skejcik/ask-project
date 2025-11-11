#include "utils.h"
#include "boost/multiprecision/float128.hpp"
#include <iostream>
#include <string>

using namespace std;
using namespace boost::multiprecision;

string binary(int n) {
  string bits = "";

  int current = n;

  while (current != 0) {
    int remainder = current % 2;
    bits = to_string(remainder) + bits;
    current = current / 2;
  }

  return bits;
}

string fraction(float128 num, int precision) {
  string result = "";

  // Gdyby użyć double to może zabraknąć precyzji i po ~52 iteracjach totalnie
  // się psuje
  //
  // Niby można sprawdzić czy po jakimś czasie się nie powtarza ale mi sie nie
  // chce
  if (num == 0) {
    for (int i = 0; i < precision; i++) {
      result += "0";
    }
    return result;
  }

  int iterations = 0;

  for (int i = 0; i < precision; i++) {
    num = num * 2;
    if (num >= 1) {
      result += "1";
      num = num - 1;
    } else {
      result += "0";
    }
    iterations++;
  }

  return result;
}

int exponent(string base) {
  int shifts = base.length() - 1;

  return shifts;
}

string ieee74(float128 digit, string bits, int exp, int precision, int bias) {
  int sign = (digit < 0) ? 1 : 0;

  digit = abs(digit);

  int exponent_bits = 15;
  int mantissa_bits = 112;

  int fraction_count = mantissa_bits - static_cast<int>(bits.length());
  string fraction_str = fraction(digit - int(digit), fraction_count);

  cout << "Fraction count: " << fraction_count << endl;

  int exp_decimal = bias + exp;
  string exp_str = binary(exp_decimal);

  while (static_cast<int>(exp_str.length()) < exponent_bits) {
    exp_str = "0" + exp_str;
  }

  cout << "Exponent string length: " << exp_str.length() << endl;
  cout << "Fraction string length: " << fraction_str.length() << endl;

  cout << "Bits: " << bits << endl;
  cout << "Fraction: " << fraction_str << ", length: " << fraction_str.length()
       << endl;

  string mantissa_str = bits + fraction_str;

  return to_string(sign) + exp_str + mantissa_str;
}
