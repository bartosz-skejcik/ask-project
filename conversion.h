#ifndef CONVERSION_H
#define CONVERSION_H

#include <string>

namespace conversion {

using namespace std;

struct ConversionResult {
  bool success = false;
  string errorMessage;
  string originalInput;
  string integerPart = "0";
  string integerBits = "0";
  int exponentValue = 0;
  string signBit = "0";
  string exponentBits = string(15, '0');
  string mantissaBits = string(112, '0');
  string ieeeBits = string(128, '0');
  bool isZero = false;
  bool isNegative = false;
};

ConversionResult ConvertToIEEE(const string &rawInput);

} // namespace conversion

#endif
