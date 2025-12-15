#include "conversion.h"
#include "boost/multiprecision/float128.hpp"
#include "utils.h"
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace conversion {

using namespace std;
using namespace boost::multiprecision;

namespace {

string Trim(const string &text) {
  size_t start = 0;
  while (start < text.size() &&
         isspace(static_cast<unsigned char>(text[start]))) {
    start++;
  }

  if (start == text.size()) {
    return "";
  }

  size_t end = text.size() - 1;
  while (end > start && isspace(static_cast<unsigned char>(text[end]))) {
    end--;
  }

  return text.substr(start, end - start + 1);
}

} // namespace

ConversionResult ConvertToIEEE(const string &rawInput) {
  ConversionResult result;
  result.originalInput = rawInput;

  string trimmed = Trim(rawInput);
  if (trimmed.empty()) {
    result.errorMessage = "Wpisz liczbę dziesiętną.";
    return result;
  }

  try {
    float128 value(trimmed);
    result.isZero = (value == 0);
    result.isNegative = (value < 0);

    float128 absValue = abs(value);
    int integerPart = static_cast<int>(absValue);
    result.integerPart = integerPart;
    result.integerBits = binary(integerPart);

    if (result.isZero) {
      result.success = true;
      result.signBit = result.isNegative ? "1" : "0";
      result.exponentBits = string(15, '0');
      result.mantissaBits = string(112, '0');
      result.ieeeBits =
          result.signBit + result.exponentBits + result.mantissaBits;
      result.exponentValue = 0;
      return result;
    }

    if (integerPart == 0) {
      result.errorMessage =
          "Wartości o |x| < 1 nie są jeszcze obsługiwane przez ten konwerter.";
      return result;
    }

    int exp = exponent(result.integerBits);
    result.exponentValue = exp;

    string ieee = ieee74(value, result.integerBits.substr(1), exp, 128);

    if (ieee.length() < 128) {
      ieee += string(128 - ieee.length(), '0');
    } else if (ieee.length() > 128) {
      ieee = ieee.substr(0, 128);
    }

    result.signBit = ieee.substr(0, 1);
    result.exponentBits = ieee.substr(1, 15);
    result.mantissaBits = ieee.substr(16);
    result.ieeeBits = ieee;
    result.success = true;
  } catch (const std::exception &) {
    result.errorMessage = "Nie można odczytać liczby. Sprawdź format.";
  }

  return result;
}

} // namespace conversion
