#include "boost/multiprecision/float128.hpp"
#include "utils.h"
#include <iostream>

using namespace std;
using namespace boost::multiprecision;

int main() {
  string input;
  cin >> input;

  float128 n(input.c_str());

  int n_int = int(abs(n));

  std::string bits = binary(n_int);

  cout << "The binary representation of " << n_int << " is " << bits << endl;

  int exp = exponent(bits);

  cout << "The exponent for base 2 is " << exp << endl;

  string ieee = ieee74(n, bits.substr(1), exp, 128);

  cout << "The IEEE 754 representation is ";

  for (size_t i = 0; i < ieee.length(); i++) {
    if (i == 0) {
      // sign bit
      cout << "\033[41m" << ieee[i] << "\033[0m";
    } else if (i > 0 && i <= 15) {
      // exponent bits
      cout << "\033[44m" << ieee[i] << "\033[0m";
    } else {
      // mantissa bits
      cout << "\033[42m" << ieee[i] << "\033[0m";
    }
  }

  cout << endl << "The length is: " << ieee.length() << endl;

  return 0;
}
