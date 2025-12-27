#include "conversion.h"
#include "constants.h"
#include "utils.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <stdexcept>

/* Do poprawnego zaokrąglenia zgodnym z IEEE 754 trzeba znać bity po mantysie. Bit Sticky jest ustawiony, kiedy po 
 * następnych dwóch bitach za mantysą jest co najmniej jedna 1. Obliczenie tylko kolejnych 100 bitów może być nie
 * najlepszym rozwiązaniem, ale powinno starczyć dla liczb obsługiwanych przez ten konwerter.
 */
#ifndef FRACTION_ROUNDING_BITS
#define FRACTION_ROUNDING_BITS 100
#endif

namespace conversion {

using namespace std;

static void RoundMantissa(string &mantissa, int &exponent,const string &integerBits, const string &fractionBits, const int &fractionLeadingZeroesCount);
static int ExponentFromInteger(string &n);

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

    string integerPart = normalizedInput.substr(0, dotPosition); // bez '-' na początku
    if (result.isNegative) integerPart = integerPart.substr(1);

    string fractionPart; // tylko samy cyfry ułamku, bez "0."
    if (dotPosition == string::npos) fractionPart = ""; // "" jeśli niema ułamku
    else fractionPart = normalizedInput.substr(dotPosition + 1);

    // dla zero
    result.isZero = (integerPart.length() == 0 || isZeroesString(integerPart))
                 && (fractionPart.length() == 0 || isZeroesString(fractionPart));
    if (result.isZero) {
      result.success = true;
      result.signBit = result.isNegative ? "1" : "0"; // W IEEE 754 istnieje wartość -0.0
      result.exponentBits = string(EXPONENT_LEN, '0');
      result.mantissaBits = string(MANTISSA_LEN, '0');
      result.ieeeBits =
          result.signBit + result.exponentBits + result.mantissaBits;
      result.exponentValue = 0;

      return result;
    }

    string integerBits = Binary(integerPart);
    result.integerPart = integerPart;
    result.integerBits = integerBits.substr(0, min<size_t>(MANTISSA_LEN + 1, integerBits.length()));

    int fractionLeadingZeroesCount = 0; // dla liczb typu 0,0...01b

    // w tej zmiennej mogą być wiodące "0" i będzie niejawna 1, używa się do zaokrąglenia mantysy
    string fractionBitsRaw = Fraction(
      fractionPart,
      MANTISSA_LEN + FRACTION_ROUNDING_BITS + 1, // +1 dla niejawnej jedynki +bity po mantysie do zaokrąglenia
      &fractionLeadingZeroesCount
    );

    // w tej zmiennej będzie ułamek bez wiodących "0" oraz niejawnej 1, który zostanie przepisany do mantysy
    string fractionBits = fractionBitsRaw;
    if (integerPart.length() == 0 || isZeroesString(integerPart)) {
    // przesunięcie bitów dla liczb typu 0,0...01
      fractionBits = fractionBits.substr(fractionLeadingZeroesCount + 1); // + 1 dla niejawnej jedynki
    }


    // wpisanie liczby całej do mantysy bez niejawnej 1 na początku
    if (result.integerBits.length() > 0 && !isZeroesString(result.integerBits))
      result.mantissaBits = result.integerBits.substr(
        1, // niejawna 1 jest ominięta
        min<size_t>(result.integerBits.length(), MANTISSA_LEN)
      );
    else result.mantissaBits = ""; // mantysa będzie złożona tylko z ułamku

    if (result.mantissaBits.length() < MANTISSA_LEN) // dopełnienie mantysy częścią ułamkową
      result.mantissaBits += fractionBits.substr(
        0,
        min<size_t>(fractionBits.length(), MANTISSA_LEN - result.mantissaBits.length())
      );

    result.mantissaBits = result.mantissaBits.substr(0, min<size_t>(result.mantissaBits.length(), MANTISSA_LEN));
    if (result.mantissaBits.length() < MANTISSA_LEN)
      result.mantissaBits += string(MANTISSA_LEN - result.mantissaBits.length(), '0'); // zera na końcu, żeby było 112 bit


    // Dla liczb >= 1 wykładnik zależy od pozycji pierwszej jedynki w części całkowitej
    // Dla liczb 0 < x < 1 wykładnik zależy od liczby zer przed pierwszą jedynką w części ułamkowej
    int exp;
    if (integerPart.length() == 0 || isZeroesString(integerPart)) {
      exp = -(fractionLeadingZeroesCount + 1);
    } else {
      exp = ExponentFromInteger(integerBits);
    }


    RoundMantissa(result.mantissaBits, exp, integerBits, fractionBitsRaw, fractionLeadingZeroesCount);


    string exponentBits = Binary(to_string(exp + EXPONENT_BIAS));
    if (exponentBits.length() < EXPONENT_LEN)
      exponentBits = string(EXPONENT_LEN - exponentBits.length(), '0') + exponentBits; // zera na początku, jeśli niema 15 bit
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

/**
 * Zaokrągla mantysę zgodnie z zasadą "round ties to even".
 * Przyjmuje referencję na (string)       mantysę, którą zaokrągli jeśli uzna to za potrzebne.
 * Przyjmuje referencję na (int)          wykładnik, który może być inkryminowany, jeśli zaokrąglenie mantysy spowoduje jej przepełnienie.
 * Przyjmuje referencję na (const string) bity części całej liczby.
 * Przyjmuje referencję na (const string) bity ułamku liczby bez "0." z wiodącymi 0.
 * Przyjmuje referencję na (const int)    liczność wiodących zer na początku ułamku.
 * Nic nie zwraca.
 */
static void RoundMantissa(
  string &mantissa, int &exponent,
  const string &integerBits, const string &fractionBits, const int &fractionLeadingZeroesCount
)
{
  bool noIntegerPart = false;
  if (integerBits.length() == 0 || isZeroesString(integerBits)) noIntegerPart = true;

  bool bitLSB = mantissa.back() == '1'; // ostatni bit mantysy
  bool bitGround = false;               // kolejny bit po LSB mantysy
  bool bitRound = false;                // kolejny bit po bitGround
  bool bitSticky = false;               // jeśli jest co najmniej jedna 1 w pozostałych bitach


  string extendedMantissaBits; // nieobcięte bity mantysy + niejawne 1 na początku
  if (!noIntegerPart) {
    extendedMantissaBits = integerBits + fractionBits;
  } else {
    extendedMantissaBits = fractionBits.substr(fractionLeadingZeroesCount);
  }

  size_t bitGroundIdx = MANTISSA_LEN + 1; // indeksacja od 0, +1 dla niejawnej jedynki

  if (bitGroundIdx < extendedMantissaBits.length())
    bitGround = extendedMantissaBits[bitGroundIdx] == '1';

  if (bitGroundIdx + 1 < extendedMantissaBits.length())
    bitRound = extendedMantissaBits[bitGroundIdx + 1] == '1';

  if (bitGroundIdx + 2 < extendedMantissaBits.length())
    bitSticky = !isZeroesString(extendedMantissaBits.substr(bitGroundIdx + 2));

  // jeśli ten warunek nie jest spełniony, nie trzeba zaokrąglać w górę
  if (!(bitGround && (bitRound || bitSticky || bitLSB))) return;

  // zaokrąglenie w górę, czyli dodanie 1 do mantysy
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
  if (carry) exponent += 1; // jeśli przepełniono mantysę, trzeba dodać 1 do wykładnika
}

/**
 * Oblicza wykładnik IEEE754 dla liczb |n| >= 1 z części całej liczby binarnej podanej jako string.
 * Przyjmuje string zawierający bity części całej liczby.
 * Zwraca wykładnik do IEEE754 bez BIAS.
 */
static int ExponentFromInteger(string &n) {
  int shifts = n.length() - 1;

  return shifts;
}

} // namespace conversion
