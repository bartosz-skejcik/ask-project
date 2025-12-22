#include <cassert>
#include <cstddef>
#include <iostream>
#include "utils.h"
#include "conversion.h"

#define FG_RED "\x1b[31m"
#define FG_GREEN "\x1b[32m"
#define FG_RESET "\x1b[39m"

static size_t testCounter = 1;
static size_t failedCounter = 0;

using namespace std;
using namespace conversion;

static bool TestConversion(string input, string expected) {
    assert(isBinaryString(expected));
    assert(expected.length() == 128);

    cout << "Running test " << testCounter++ << "\t";

    ConversionResult result = ConvertToIEEE(input);
    if (!result.success) {
        cout << FG_RED << "Błąd konwersji" << FG_RESET << " " << input << endl;
        failedCounter++;
        return false;
    }

    if (result.ieeeBits != expected) {
        cout << FG_RED << "FAILED" << FG_RESET << endl;
        cout << "Dla liczby " << input << " oczekiwano inny wynik" << endl;
        cout << result.ieeeBits << " Otrzymano" << endl;
        cout << expected << " Oczekiwano" << endl << endl;

        failedCounter++;
        return false;
    }

    cout << FG_GREEN << "OK" << FG_RESET << " " << input << endl;
    return true;
}

int main (int argc, char *argv[]) {

    cout << "Początek testowania" << endl << endl;

    TestConversion(
            "420.750",
            "01000000000001111010010011000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    );

    TestConversion(
            "-420.750",
            "11000000000001111010010011000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    );

    TestConversion( // mantysa jest złożona tylko z części całej liczby
            "9999999999999999999999999999999999999999.99999999999999999999999999999",
            "01000000100000111101011000110010100111110001110000110101110010100100101111111010101110111001111101010110000100000000000000000000"
    );
    TestConversion(
            "9999999999999999999999999999999999999999",
            "01000000100000111101011000110010100111110001110000110101110010100100101111111010101110111001111101010110000100000000000000000000"
    );

    TestConversion( // mantysa jest złożona tylko z części ułamkowej liczby
            "0.0000000000000000000000000000001",
            "00111111100110000000001110011101011001100101100010010110100001111111100111101001000000011101010110011111001010010000111011100010"
    );

    TestConversion( // testowanie poprawnego przesunięcia ułamku
            "0.0078125", // 127 zer i jedna 1 na końcu. 1 będzie niejawna, wykładnik się zmieni
            "00111111111110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    );

    TestConversion( // do wykonania testu musi wykonywać się poprawne zaokrąglenie
            "105.3",
            "01000000000001011010010100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011"
    );

    TestConversion(
            "0.0",
            "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    );

    TestConversion(
            "-0.0",
            "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    );

    cout << endl << "\t" << testCounter - 1 << " testów przeprowadzono" << endl;
    cout << "\t" << FG_GREEN << testCounter - 1 - failedCounter << " PASSED" << FG_RESET << "\t" << FG_RED << failedCounter << " FAILED" << FG_RESET << endl;

    return 0;
}
