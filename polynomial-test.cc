// Copyright (c) 2020 Chanjung Kim. All rights reserved.

#include "polynomial.hh"
#include <iostream>
#include <string>

using namespace std;

int main()
{
    while (true)
    {
        try
        {
            cout << "p = ";
            string pStr;
            getline(cin, pStr);
            if (pStr.empty()) break;
            auto p = Polynomial<double>::FromString(pStr);

            cout << "q = ";
            string qStr;
            getline(cin, qStr);
            if (qStr.empty()) break;
            auto q = Polynomial<double>::FromString(qStr);

            cout << "p = " << p << endl;
            cout << "q = " << q << endl;
            cout << "p + q = " << p + q << endl;
            cout << "p - q = " << p - q << endl;
            cout << "p * q = " << p * q << endl;
            cout << "2p = " << 2 * p << endl;
            cout << "2q = " << 2 * q << endl;
            cout << u8"p² = " << pow(p, 2) << endl;
            cout << u8"p³ = " << pow(p, 3) << endl;
            cout << u8"q² = " << pow(q, 2) << endl;
            cout << u8"q³ = " << pow(q, 3) << endl;
        }
        catch (std::invalid_argument)
        {
            cout << "Invalid polynomial!" << endl;
        }
    }
}