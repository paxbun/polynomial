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
            auto p = Polynomial<'z'>::FromString(pStr);

            cout << "q = ";
            string qStr;
            getline(cin, qStr);
            if (qStr.empty()) break;
            auto q = Polynomial<'z'>::FromString(qStr);

            cout << "p = " << p << endl;
            cout << "q = " << q << endl;
            cout << u8"∫ p d" << p.Variable << " = " << p.Integral() << endl;
            cout << u8"∫ q d" << q.Variable << " = " << q.Integral() << endl;
            cout << "dp/d" << p.Variable << " = " << p.Diff() << endl;
            cout << "dq/d" << q.Variable << " = " << q.Diff() << endl;
            cout << "p(5) = " << p(5) << endl;
            cout << "q(2.4) = " << q(2.4) << endl;
            cout << "p + q = " << p + q << endl;
            cout << "p - q = " << p - q << endl;
            cout << "p * q = " << p * q << endl;
            cout << "2p = " << 2 * p << endl;
            cout << "2q = " << 2 * q << endl;
            cout << u8"p² = " << pow(p, 2) << endl;
            cout << u8"p³ = " << pow(p, 3) << endl;
            cout << u8"p(5)² = " << pow(p, 2)(5) << endl;
            cout << u8"q² = " << pow(q, 2) << endl;
            cout << u8"q³ = " << pow(q, 3) << endl;
            cout << u8"q(2.4)³ = " << pow(q, 3)(2.4) << endl;
        }
        catch (std::invalid_argument)
        {
            cout << "Invalid polynomial!" << endl;
        }
    }
}