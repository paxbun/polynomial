// Copyright (c) 2020 Chanjung Kim. All rights reserved.

#include "polynomial.hh"
#include <iostream>
#include <string>

using namespace std;

int main()
{
    while (true)
    {
        cout << "p = ";
        string str;
        getline(cin, str);
        if (str.empty()) break;

        auto p = Polynomial<double>::FromString(str);
        cout << "p = " << p << endl;
        cout << u8"p² = " << pow(p, 2) << endl;
        cout << u8"p³ = " << pow(p, 3) << endl;
    }
}