#include "conversion.h"
#include "constants.h"
#include "utils.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <stdexcept>

namespace conversion {

using namespace std;

namespace {

} // namespace

static void RoundMantissa(string &mantissa, int &exponent,const string &integerBits, const string &fractionBits, const int &fractionLeadingZeroesCount);

// sprawdza format podanej liczby
static bool ValidateInput(const string &rawInput) {
  if (rawInput.length() == 0) return false;

  bool is_fraction = false;
  bool is_negative = (rawInput[0] == '-') ? true : false;

  for (char c : (is_negative) ? rawInput.substr(1) : rawInput) {
    if (isdigit(c)) continue;
    else if (c == '.' || c == ',') {
      if (is_fraction) return false;
      else is_fraction = true;
    }
    else return false;
  }
  return true;
}

ConversionResult ConvertToIEEE(const string &rawInput) {
  ConversionResult result;

  if (!ValidateInput(rawInput)) {
    result.errorMessage = "Nie można odczytać liczby. Sprawdź format.";
    return result;
  }

  result.originalInput = rawInput;

  string normalizedInput = Trim(rawInput);
  replace(normalizedInput.begin(), normalizedInput.end(), ',', '.'); // przecinek też działa
  if (normalizedInput.empty()) {
    result.errorMessage = "Wpisz liczbę dziesiętną.";
    return result;
  }

  try {
    result.isNegative = (normalizedInput.length() > 0 && normalizedInput[0] == '-');
    size_t dotPosition = normalizedInput.find('.');

    string integerPart = normalizedInput.substr(0, dotPosition);
    if (result.isNegative) integerPart = integerPart.substr(1);

    string fractionPart;
    if (dotPosition == string::npos) fractionPart = "";
    else fractionPart = normalizedInput.substr(dotPosition + 1);

    // dla zero
    result.isZero = (integerPart.length() == 0 || isZeroesString(integerPart))
                 && (fractionPart.length() == 0 || isZeroesString(fractionPart));
    if (result.isZero) {
      result.success = true;
      result.signBit = result.isNegative ? "1" : "0";
      result.exponentBits = string(EXPONENT_LEN, '0');
      result.mantissaBits = string(MANTISSA_LEN, '0');
      result.ieeeBits =
          result.signBit + result.exponentBits + result.mantissaBits;
      result.exponentValue = 0;

      return result;
    }

    string integerBits = binary(integerPart);
    result.integerPart = integerPart;
    result.integerBits = integerBits.substr(0, min<size_t>(MANTISSA_LEN + 1, integerBits.length()));

    int fractionLeadingZeroesCount = 0; // dla liczb typu 0,0...01b
    string fractionBitsRaw = fraction(
      fractionPart,
      MANTISSA_LEN + 2, // +1 dla niejawnej jedynki, +1 dla otrzymania bitu od razu za LSB do poprawnego zaokrąglenia
      &fractionLeadingZeroesCount
    );
    // przesunięcie mantysy dla liczb typu 0,0...01
    string fractionBits = fractionBitsRaw;
    if (integerPart.length() == 0 || isZeroesString(integerPart)) {
      fractionBits = fractionBits.substr(fractionLeadingZeroesCount + 1); // zawsze +1 dla niejawnej jedynki
    }


    // wpisanie liczby całej do mantysy bez niejawnej 1 na początku
    if (result.integerBits.length() > 0 && !isZeroesString(result.integerBits))
      result.mantissaBits = result.integerBits.substr(1, // niejawna 1 jest ominięta
          min<size_t>(result.integerBits.length(), MANTISSA_LEN + 1)
      );
    else result.mantissaBits = ""; // mantysa będzie złożona tylko z ułamku

    if (result.mantissaBits.length() < MANTISSA_LEN) // dopełnienie mantysy częścią ułamkową
      result.mantissaBits += fractionBits.substr(0, min<size_t>(fractionBits.length(), MANTISSA_LEN - result.mantissaBits.length()));
    result.mantissaBits = result.mantissaBits.substr(0, min<size_t>(result.mantissaBits.length(), MANTISSA_LEN));

    if (result.mantissaBits.length() < MANTISSA_LEN)
      result.mantissaBits += string(MANTISSA_LEN - result.mantissaBits.length(), '0'); // zera na końcu, żeby było 112 bit


    // Dla liczb >= 1 wykładnik zależy od pozycji pierwszej jedynki w części całkowitej
    // Dla liczb 0 < x < 1 wykładnik zależy od liczby zer przed pierwszą jedynką w części ułamkowej
    int exp;
    if (integerPart.length() == 0 || isZeroesString(integerPart)) {
      exp = -(fractionLeadingZeroesCount + 1);
    } else {
      exp = exponentFromInteger(integerBits);
    }


    RoundMantissa(result.mantissaBits, exp, integerBits, fractionBitsRaw, fractionLeadingZeroesCount);


    string exponentBits = binary(to_string(exp + EXPONENT_BIAS));
    if (exponentBits.length() < EXPONENT_LEN)
      exponentBits = string(EXPONENT_LEN - exponentBits.length(), '0') + exponentBits;
    exponentBits = exponentBits.substr(0, min<size_t>(exponentBits.length(), EXPONENT_LEN));


    result.signBit = (result.isNegative ? "1" : "0");
    result.exponentValue = exp;
    result.exponentBits = exponentBits;
    result.ieeeBits = result.signBit + result.exponentBits + result.mantissaBits;
    result.success = true;

    assert(result.signBit.length() == 1);
    assert(result.exponentBits.length() == EXPONENT_LEN);
    assert(result.mantissaBits.length() == MANTISSA_LEN);
    assert(result.ieeeBits.length() == IEEE_LEN);

  } catch (const std::exception &) {
    result.errorMessage = "Nie można odczytać liczby. Sprawdź format.";
  }

  return result;
}

static void RoundMantissa(
  string &mantissa, int &exponent,
  const string &integerBits, const string &fractionBits, const int &fractionLeadingZeroesCount
) {
  bool noIntegerPart = false;
  if (integerBits.length() == 0 || isZeroesString(integerBits)) noIntegerPart = true;

  bool bitBehindLSB = false;
  if (
      ((integerBits.length() > MANTISSA_LEN) && integerBits[MANTISSA_LEN + 1] == '1' )
        || ((fractionBits.length() > MANTISSA_LEN + 1) &&
          fractionBits[
          (noIntegerPart ? (fractionLeadingZeroesCount + MANTISSA_LEN + 1)
           : (MANTISSA_LEN + 1 - integerBits.length())
           )] == '1'
        )
      ) {
      bitBehindLSB = true;
  }

  if (bitBehindLSB) { // dodanie 1 do mantysy
    bool carry = true;
    for (size_t i = mantissa.length(); i != 0; i--) {
      if (!carry) break;
      if (mantissa[i - 1] == '1') {
        mantissa[i - 1] = '0';
      } else {
        mantissa[i - 1] = '1';
        carry = false;
      }
    }
    if (carry) exponent += 1;
  }
}

} // namespace conversion
