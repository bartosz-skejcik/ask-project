#include "utils.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <string>

using namespace std;

bool isBinaryString(const string &n) {
  for (char c : n)
    if (!(c == '0' || c == '1')) return false;
  return true;
}

bool isNumberString(const string &n) {
  for (char c : n) if (!isdigit(c)) return false;
  return true;
}

bool isZeroesString(string n) {
  for (char c : n) if (c != '0')
      return false;
  return true;
}

/**
 * Konwertuje zapis liczby z systemu dziesiętnego do binarnego.
 * Przyjmuje cześć całkowitą liczby jako string, działa na dużych liczbach.
 * Zwraca ciąg znaków '0' oraz '1'.
 */
string Binary(string n) {
  string bits = "";
  string n_half = "";
  bool carry;

  assert(isNumberString(n));
  if (n == "") return "";
  n = Trim(n);

  while (n.length() > 0 && !isZeroesString(n)) {
    // dzielenie przez 2 każdej cyfry podanej liczby
    carry = false;
    n_half = "";
    for (char c : n) {
      int digit = static_cast<int>(c - '0');
      if (carry) // przesunięcie z poprzedniego dzielenia
        digit += 10;

      int digit_half = digit / 2;
      carry = (digit % 2) == 1;

      n_half.push_back(digit_half + '0');
    }

    bits.push_back(carry ? '1' : '0');
    n = n_half;
  }

  reverse(bits.begin(), bits.end());
  return bits;
}

/**
 * Konwertuje zapis ułamku z systemu dziesiętnego do binarnego.
 * Przyjmuje ułamek jako string bez "0."
 * Przyjmuje precyzyjność jako size_t.
 * Przyjmuje pointera na int, do którego będzie zapisana liczność zer na początku liczby, jeśli on nie jest nullptr.
 * Zwraca string z reprezentacją ułamku zawierający tylko znaki '0' oraz '1' o długości ograniczonej do precision + ilość zer na początku.
 */
string Fraction(string frac, size_t precision, int *pLeadingZeroes) {
  string result = "";
  size_t leadingZeroes = 0;
  bool countingLeadingZeroes = true;

  assert(isNumberString(frac));
  if (frac == "") return "";
  frac = Trim(frac);

  for (size_t _ = 0; _ < precision + leadingZeroes; _++) {
    string doubled = "";
    bool carry = false;
    for (size_t i = frac.length(); i != 0; i--) {
      // w pętli wykonuję się mnożenie przez 2 każdej cyfry podanej liczby, zaczynając z końca
      int digit = static_cast<int>(frac[i - 1] - '0');

      digit *= 2;
      if (carry) digit++;

      if (digit >= 10)
        carry = true;
      else
        carry = false;

      doubled += (digit % 10) + '0';
    }

    result.append(carry ? "1" : "0"); // jeśli w tym miejscu jest przesunięcie, to wynik mnożenia przez 2 jest >= 1
    if (carry) countingLeadingZeroes = false;
    if (!carry && countingLeadingZeroes) leadingZeroes++;

    reverse(doubled.begin(), doubled.end());
    frac = doubled;

    if (isZeroesString(frac)) break;
  }

  if (pLeadingZeroes != nullptr)
    *pLeadingZeroes = leadingZeroes;

  return result;
}

/**
 * Oblicza wykładnik IEEE754 dla liczb |n| > 1 z części całej liczby binarnej podanej jako string.
 */
int ExponentFromInteger(string &base) {
  int shifts = base.length() - 1;

  return shifts;
}

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
