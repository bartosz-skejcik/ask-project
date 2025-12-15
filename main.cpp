#include "conversion.h"
#include "gui.h"
#include <iostream>

using conversion::ConversionResult;
using conversion::ConvertToIEEE;
using gui::RunGuiMode;
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::string;

namespace {

void RunCliMode() {
  string input;
  if (!(cin >> input)) {
    cerr << "Nie udało się odczytać danych wejściowych." << endl;
    return;
  }

  ConversionResult result = ConvertToIEEE(input);
  if (!result.success) {
    cerr << result.errorMessage << endl;
    return;
  }

    cout << "Reprezentacja binarna liczby " << result.integerPart << " to "
      << result.integerBits << endl;
    cout << "Wykładnik dla podstawy 2 to " << result.exponentValue << endl;

    cout << "Reprezentacja IEEE 754 to ";
  for (size_t i = 0; i < result.ieeeBits.length(); i++) {
    if (i == 0) {
      cout << "\033[41m" << result.ieeeBits[i] << "\033[0m";
    } else if (i > 0 && i <= 15) {
      cout << "\033[44m" << result.ieeeBits[i] << "\033[0m";
    } else {
      cout << "\033[42m" << result.ieeeBits[i] << "\033[0m";
    }
  }

  cout << endl << "Długość: " << result.ieeeBits.length() << endl;
}

} // namespace

int main(int argc, char **argv) {
  if (argc > 1 && string(argv[1]) == "--cli") {
    RunCliMode();
    return 0;
  }

  return RunGuiMode(argc, argv);
}
