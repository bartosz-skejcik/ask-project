#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <cstddef>

using namespace std;

string binary(string n);

string fraction(string frac, size_t precision, int *pexpshift = nullptr);

int exponentFromInteger(string &base);

string Trim(const string &text);

bool isZeroesString(string n);
bool isBinaryString(const string &n);
bool isNumberString(const string &n);

#endif
