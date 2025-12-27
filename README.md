# Kalkulator Reprezentacji IEEE 754

Ten projekt konwertuje liczby dziesiętne na ich binarną reprezentację zgodną z IEEE 754 w precyzji poczwórnej (128-bit) i oferuje interfejs graficzny zbudowany na Qt, który wizualizuje kolejne etapy konwersji.

## Funkcjonalności

-   Konwersja liczb dziesiętnych na binarne
-   Obliczanie formatu IEEE 754 w precyzji poczwórnej (128-bit)
-   Kolorowe oznaczenie wyniku (bit znaku na czerwono, wykładnik na niebiesko, mantysa na zielono)
-   Lekki interfejs graficzny (Qt) z polem tekstowym, przyciskami **Konwertuj** i **Wyczyść** oraz wizualizacją segmentów (bit znaku / wykładnik / mantysa)

## Wymagania

### Wszystkie systemy operacyjne

Potrzebujesz:

-   Kompilatora zgodnego z C++17 (g++ 7+ lub clang++ 5+)
-   Biblioteki [Qt](https://www.qt.io/) w wersji 5.12+ (nagłówki + biblioteki)
-   Narzędzia `make` do budowania

### Linux (Debian/Ubuntu)

```bash
sudo apt update
sudo apt install build-essential make qtbase5-dev qtbase5-dev-tools
```

### Linux (Fedora/RHEL)

```bash
sudo dnf install gcc-c++ make qt5-devel
```

### Linux (Arch)

```bash
sudo pacman -S base-devel make qt5-base
```

### macOS

Zainstaluj Xcode Command Line Tools i Homebrew.

```bash
brew install gcc
```

Następnie zaktualizuj Makefile, aby używał `g++-13` (lub Twojej wersji GCC) zamiast `g++`.

### Windows

**Opcja 1: WSL (Zalecane)**

-   Zainstaluj WSL2 i Ubuntu, następnie postępuj zgodnie z instrukcjami dla Linuxa powyżej

**Opcja 2: MinGW-w64**

1. Zainstaluj [MinGW-w64](https://www.mingw-w64.org/), [GNU Make](https://www.gnu.org/software/make/) oraz pkg-config (np. [pkgconfiglite](https://sourceforge.net/projects/pkgconfiglite/).
2. Zainstaluj Qt (np. `vcpkg install qt5-base` czy `pacman -S mingw-w64-ucrt-x86_64-qt5-base` w konsoli **MSYS2**).
3. (Opcjonalnie) do automatycznego eksportu wszystkich wymaganych plików DLL jest wymagane narzędzie `ntldd.exe` oraz PowerShell w wersji 5+. Można pobrać `ntldd.exe` za pomocą konsoli MSYS2 (`pacman -S mingw-w64-ucrt-x86_64-ntldd`).

**Opcja 3: Visual Studio**

-   Zainstaluj Visual Studio z obsługą C++
-   Będziesz musiał stworzyć projekt VS lub dostosować komendy budowania

## Instalacja

1. Sklonuj repozytorium:

```bash
git clone https://github.com/bartosz-skejcik/ask-project.git
cd ask-project
```

2. (Windows MinGW-w64) Skonfiguruj pkg-config i sprawdź instalację Qt

```ps1
# Na przykładzie Qt zainstalowanego przez MSYS2 (mingw-w64-ucrt-x86_64-qt5-base) w katalogu "C:\msys64"
$env:PKG_CONFIG_PATH = "C:\msys64\ucrt64\lib\pkgconfig\"

# Można sprawdzić czy pkg-config wyświetli potrzebne opcji dla Qt
pkg-config --libs Qt5Widgets
# -lQt5Widgets -lQt5Gui -lQt5Core
```

## Kompilacja

```bash
make
```

Lub zbuduj i uruchom od razu (domyślnie uruchamia GUI):

```bash
make run
```

### Wdrożenie aplikacji na platformę Windows

#### Konsolidacja statyczna - jeden plik (Zalecane)

Można dodać wszystkie biblioteki Qt (wraz z ich zależnościami) do pliku binarnego aplikacji. Dla tego musi być zainstalowany **qt-static** (np. paczka `mingw-w64-ucrt-x86_64-qt5-static` w MSYS2).

```bash
# W konsoli MSYS2
pacman -S mingw-w64-ucrt-x86_64-qt5-static
```

**Jest konieczna** rekompilacja całego projektu.

```bash
make clean
```

Następnie używając qmake **Z STATYCZNEJ INSTALACJI QT**, trzeba stworzyć `Makefile.qmake` i użyć go do kompilacji projektu.

```bash
C:\msys64\ucrt64\qt5-static\bin\qmake -o Makefile.qmake qmake_static.pro

make -f Makefile.qmake
```

Po udanej kompilacji w katalogu `bin` będzie znajdował się plik `static_main.exe`. Ten plik może być wysłany i uruchomiony na komputerze, który nie ma zainstalowanego Qt.

#### Konsolidacja dynamiczna - eksport zależności

Żeby użytkownicy aplikacji mogli użyć ją bez potrzeby zainstalowania na system wszystkich niezbędnych bibliotek, można zapakować je razem z aplikacją.Do tego na systemie muszą być zainstalowane: `windeploy-qt`, `ntldd` oraz `PowerShell` w wersji 5+.

```bash
make clean windeploy
```

## Użycie

### Interfejs graficzny (domyślny)

```bash
./bin/main
```

GUI wyświetla pole tekstowe, przyciski **Konwertuj**/**Wyczyść** oraz panele z wizualizacją bitów. Wyniki aktualizują się natychmiast po naciśnięciu przycisku Konwertuj lub Enter (na klawiaturze).

### Tryb konsolowy

```bash
./bin/main --cli
```

Tryb ten zachowuje dotychczasowe kolorowanie znaków w terminalu.

Domyślnie na windowsie ten tryb nie jest dostępny. Wsparcie trybu konsolowego wymaga otwierania konsoli za każdym razem, kiedy aplikacja jest uruchomiona (nawet w trybie graficznym). Żeby skompilować projekt z wsparciem trybu konsolowego trzeba podać do `make` parametr `WINCLISUPPORT`

```bash
make WINCLISUPPORT=1
```

## Czyszczenie

Usuń skompilowane pliki:

```bash
make clean
```

## Automatyczne testowania

```bash
make runtest
```

Nowe testy można dodawać do **test.cpp**.

## Rozwiązywanie problemów

### "Package Qt5Widgets was not found in the pkg-config search path. Perhaps you should add the directory containing `Qt5Widgets.pc'"

pkg-config nie jest dobrze skonfigurowany, lub instalacja Qt nie zawiera pliku Qt5Widgets.pc w katalogu za ścieżką podaną w `$PKG_CONFIG_PATH`. Za potrzeby, można użyć Qt innej wersji, podając do `make` inny `QT_PKG` (na przykład `make QT_PKG="Qt6Widgets"`).

### Ostrzeżenia kompilacji o nieużywanych parametrach

To ostrzeżenia niekrytyczne i mogą być zignorowane. Kod będzie działał poprawnie.

## Struktura projektu

```
.
├── main.cpp                              # Główny punkt wejścia programu
├── utils.cpp                             # Funkcje pomocnicze do konwersji
├── utils.h                               # Plik nagłówkowy z deklaracjami funkcji pomocniczych
├── conversion.cpp                        # Konwerter
├── conversion.h                          # Plik nagłówkowy z deklaracjami funkcji konwertera
├── gui.cpp                               # GUI
├── gui.h                                 # Plik nagłówkowy z deklaracjami funkcji GUI
├── constants.h                           # Plik nagłówkowy z definicjami stałych
├── test.cpp                              # Testy automatyczne używane podczas zaprojektowania programu
├── windows_deploy_deep_dependencies.ps1  # Skrypt importujący pliki DLL dla wdrożenia aplikacji na Windows
├── qmake_static.pro                      # Plik do qmake, który służy do generacji Makefile do konsolidacji statycznej projektu
├── Makefile                              # Konfiguracja budowania
├── bin/                                  # Skompilowany plik wykonywalny (generowany)
└── build/                                # Pliki obiektowe (generowane)
```

## Szczegóły techniczne

-   **Precyzja**: IEEE 754 precyzja poczwórna (128-bit)
-   **Format**: 1 bit znaku + 15 bitów wykładnika + 112 bitów mantysy
-   **Bias wykładnika**: 16383
-   **Obsługiwane dane wejściowe**: Liczby dziesiętne (dodatnie i ujemne)

## Licencja

Ten projekt ma cele edukacyjne.
