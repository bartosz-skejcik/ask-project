# Opis testów rezultatu zadania projektowego

## 1. Wprowadzenie do testowania

W ramach projektu konwertera liczb dziesiętnych na format IEEE 754 (128-bit) przeprowadzono serię testów jednostkowych oraz testów manualnych interfejsu graficznego. Testy miały na celu weryfikację poprawności implementacji algorytmu konwersji oraz sprawdzenie zachowania programu w różnych przypadkach brzegowych.

## 2. Metodyka testowania

### 2.1 Testy jednostkowe

Przygotowano zestaw 46 testów automatycznych w pliku `test.cpp`, które weryfikują poprawność konwersji dla różnorodnych przypadków testowych. Każdy test porównuje wynik działania funkcji `ConvertToIEEE()` z oczekiwaną reprezentacją binarną liczby w formacie IEEE 754.

Struktura pojedynczego testu:

```cpp
TestConversion(
    "420.750",  // Liczba wejściowa
    "0100000000000111101001001100000..." // Oczekiwany wynik (128 bitów)
);
```

### 2.2 Weryfikacja wyników

Poprawność implementacji była weryfikowana za pomocą zewnętrznego narzędzia online dostępnego pod adresem: https://numeral-systems.com/ieee-754-converter/, które pozwala na krokową wizualizację procesu konwersji oraz weryfikację otrzymanych wyników.

## 3. Kategorie przypadków testowych

### 3.1 Liczby dodatnie i ujemne

**Test 1-2: Podstawowe liczby zmiennoprzecinkowe**

-   Wejście: `420.750` oraz `-420.750`
-   Cel: Weryfikacja podstawowej konwersji oraz prawidłowego ustawienia bitu znaku
-   Wynik: ✅ PASSED

**Test 3: Liczba z długą częścią ułamkową**

-   Wejście: `105.3`
-   Cel: Sprawdzenie konwersji liczby, której reprezentacja binarna części ułamkowej jest periodyczna
-   Wynik: ✅ PASSED

### 3.2 Bardzo duże liczby całkowite

**Test 4-5: Maksymalne precyzyjne liczby całkowite**

-   Wejście: `9999999999999999999999999999999999999999.99999...` i `9999999999999999999999999999999999999999`
-   Cel: Testowanie granic reprezentacji - mantysa złożona wyłącznie z części całkowitej
-   Wynik: ✅ PASSED
-   Uwaga: Oba przypadki dają identyczny wynik ze względu na ograniczenia precyzji

### 3.3 Bardzo małe liczby (0 < |x| < 1)

**Test 6: Ekstremnie mała liczba dodatnia**

-   Wejście: `0.0000000000000000000000000000001`
-   Cel: Weryfikacja konwersji liczb bliskich zeru, mantysa złożona wyłącznie z części ułamkowej
-   Wynik: ✅ PASSED

**Test 7: Precyzyjne przesunięcie ułamka**

-   Wejście: `0.0078125`
-   Cel: Testowanie liczby o reprezentacji binarnej: 127 zer + jedna jedynka (bit niejawny)
-   Wynik: ✅ PASSED
-   Znaczenie: Weryfikacja poprawnego obliczania wykładnika dla małych liczb

### 3.4 Liczby specjalne

**Test 9-10: Zero dodatnie i ujemne**

-   Wejście: `0.0` oraz `-0.0`
-   Cel: Sprawdzenie reprezentacji zera zgodnie ze standardem IEEE 754
-   Wynik: ✅ PASSED
-   Uwaga: Jedyna różnica to bit znaku

**Test 11-14: Ułamki dziesiętne niereprezentowalne dokładnie**

-   Wejście: `0.1`, `0.3`, `0.7`
-   Cel: Weryfikacja konwersji liczb, które nie mają skończonej reprezentacji binarnej
-   Wynik: ✅ PASSED
-   Obserwacja: Widoczne periodyczne powtarzanie wzoru `0011` w mantysie

### 3.5 Liczby z zakresu 1-2

**Test 19-26: Sekwencja liczb 1.1 - 1.9**

-   Wejście: `1.1`, `1.2`, `1.3`, `1.4`, `1.5`, `1.6`, `1.7`, `1.8`, `1.9`
-   Cel: Szczegółowa weryfikacja konwersji w najbardziej typowym zakresie
-   Wynik: ✅ PASSED dla wszystkich
-   Znaczenie: Ten zakres jest krytyczny dla wielu aplikacji

### 3.6 Testy zaokrąglania

**Test 31: Przypadek bez zaokrąglenia**

-   Wejście: `0.5000000000000000000000000000000000481...`
-   Cel: Weryfikacja reguły "Guard bit = 1, Round bit = 0" (brak zaokrąglenia)
-   Wynik: ✅ PASSED
-   Wyjaśnienie: Zgodnie z regułą "Round to nearest, ties to even" - nie zaokrągla się

**Test 32-34: Przypadki z zaokrągleniem**

-   Wejście: Liczby z bardzo długą częścią ułamkową przekraczającą precyzję 112 bitów
-   Cel: Sprawdzenie implementacji zaokrąglania według IEEE 754
-   Wynik: ✅ PASSED
-   Metoda: "Round to nearest, ties to even"

**Test 38-39: Granica zaokrąglenia przy wartości 2.0**

-   Wejście: `1.9999999999999999999999999999999999` (34 cyfry 9)
-   Wynik: Brak zaokrąglenia, reprezentacja `1.111...111` w systemie binarnym
-   Wejście: `1.99999999999999999999999999999999999` (35 cyfr 9)
-   Wynik: ✅ Zaokrąglenie do `2.0`
-   Znaczenie: Demonstracja precyzyjnego punktu, w którym następuje zaokrąglenie

### 3.7 Testy liczb o maksymalnej mantysie

**Test 43-50: Liczby z sekwencją jedynek w reprezentacji binarnej**

-   Wejście: Liczby postaci "111...111" (od 111 do 114 jedynek w systemie binarnym)
-   Przykłady:
    -   `2596148429267413814265248164610047` - 111 jedynek
    -   `5192296858534827628530496329220094` - 112 jedynek
    -   `10384593717069655257060992658440191` - 113 jedynek
    -   `20769187434139310514121985316880383` - 114 jedynek (wymaga zaokrąglenia)
-   Cel: Testowanie granic precyzji mantysy (112 bitów + 1 bit niejawny)
-   Wynik: ✅ PASSED dla wszystkich przypadków
-   Obserwacja: Liczby przekraczające 113 bitów precyzji są poprawnie zaokrąglane

## 4. Wyniki testów automatycznych

### 4.1 Podsumowanie wykonania testów

Po implementacji wszystkich ulepszeń, włączając:

-   Usunięcie zależności od biblioteki Boost
-   Implementację obsługi bardzo dużych liczb przy użyciu operacji na stringach
-   Implementację konwersji liczb 0 < |x| < 1
-   Poprawną implementację zaokrąglania "Round to nearest, ties to even"

Otrzymano następujący wynik:

```
Początek testowania

Test 1    OK 420.750
Test 2    OK -420.750
Test 3    OK 105.3
[...]
Test 46   OK -20769187434139310514121985316880382

        46 testów przeprowadzono
        46 PASSED    0 FAILED
        Wszystko gra!
```

**Wynik końcowy: 46/46 testów zaliczonych (100%)**

### 4.2 Problemy napotkane podczas testowania

#### Problem 1: Konwersja bardzo dużych liczb całkowitych

-   **Opis**: Pierwotna implementacja używała typu `int` do przechowywania części całkowitej
-   **Objaw**: Overflow dla liczb > 2³¹-1
-   **Rozwiązanie**: Reimplementacja funkcji `binary()` do pracy na stringach

#### Problem 2: Konwersja liczb 0 < |x| < 1

-   **Opis**: Błąd `substr(1)` na pustym stringu gdy część całkowita = 0
-   **Rozwiązanie**: Dodanie obsługi przypadku pustego stringu bitów części całkowitej

#### Problem 3: Zaokrąglanie mantysy

-   **Opis**: Brak implementacji zaokrąglania zgodnie z IEEE 754
-   **Objaw**: Niepoprawne wyniki w testach 31-34, 38-39
-   **Rozwiązanie**: Implementacja algorytmu "Round to nearest, ties to even"
    -   Analiza bitów Guard, Round i Sticky
    -   Zaokrąglenie w górę gdy Round=1 i (Guard=1 lub Sticky=1)
    -   W przypadku "tie" (Round=1, Guard=0, Sticky=0) - zaokrąglenie do parzystej

## 5. Testy interfejsu graficznego

### 5.1 Walidacja wejścia

**Test: Ograniczenie liczby znaków**

-   Implementacja: Limit 100 znaków w polu tekstowym
-   Cel: Zapobieganie zawieszeniu programu przy ekstremalnie długich liczbach
-   Wynik: ✅ Działa poprawnie - wyświetla powiadomienie przy próbie przekroczenia limitu

**Test: Walidacja formatu liczby**

-   Dopuszczalne znaki: cyfry 0-9, kropka dziesiętna, minus (na początku)
-   Cel: Zapobieganie wprowadzaniu niepoprawnych danych
-   Wynik: ✅ Niemożliwe wprowadzenie niedozwolonych znaków

### 5.2 Wyświetlanie wyników

**Test: Podział na sekcje**
Program wyświetla wynik konwersji podzielony na trzy sekcje:

-   **Bit znaku** (1 bit): `S`
-   **Wykładnik** (15 bitów): `EEEEEEEEEEEEEEE`
-   **Mantysa** (112 bitów): `MMM...MMM`

**Przykład dla liczby 420.750:**

```
Znak:      0
Wykładnik: 100000000000111
Mantysa:   1010010011000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
```

### 5.3 Kompatybilność

**Platforma: Windows**

-   Testowano na czystej maszynie wirtualnej
-   Wszystkie zależności QT prawidłowo wyeksportowane
-   Aplikacja uruchamia się bez konsoli (tylko okno GUI)
-   Status: ✅ Działa poprawnie

**Platforma: Linux**

-   Kompilacja przez Makefile z dynamicznym linkowaniem
-   Status: ✅ Działa poprawnie

**Statyczne linkowanie:**

-   Udało się utworzyć pojedynczy plik .exe zawierający wszystkie zależności
-   Metoda: qmake z odpowiednimi flagami kompilacji
-   Status: ✅ Dostępne w wersji wydania

## 6. Wnioski z testowania

1. **Poprawność implementacji**: Wszystkie 46 testów automatycznych przeszły pomyślnie, co potwierdza zgodność z standardem IEEE 754

2. **Obsługa przypadków brzegowych**: Program poprawnie radzi sobie z:

    - Bardzo dużymi liczbami całkowitymi
    - Bardzo małymi liczbami bliskimi zeru
    - Liczbami wymagającymi zaokrąglenia
    - Zerem dodatnim i ujemnym
    - Liczbami ujemnymi

3. **Stabilność**: Implementacja walidacji wejścia zapobiega zawieszeniu programu

4. **Użyteczność**: Interfejs graficzny jest przejrzysty i intuicyjny, wyniki są czytelnie podzielone na logiczne sekcje

5. **Przenośność**: Program działa zarówno na systemach Windows jak i Linux, dostępna jest wersja ze statycznym linkowaniem dla łatwej dystrybucji
