// CodeSample.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <random>
#include <numeric>
#include <memory>

#include "gnuplot-iostream.h"
//#include "SwiftRNGProDeviceEntropySource.h"

//int a, argc;

#define LARGE_VECTOR_SIZE 10000

int main()
{
    //std::cout << "Hello World!\n";

    Gnuplot gp("\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\"");

    //a = _tmain(argc);

    std::random_device rd;
    std::mt19937 mt(rd());
    std::normal_distribution<double> normdist(0.0, 1.0);

    auto vecOfVecs = std::make_shared<std::vector<std::vector<double>>>(10,std::vector<double>(LARGE_VECTOR_SIZE,0));

    std::cout << vecOfVecs->at(0)[0] << std::endl;

    for (int i = 0; i < 10; i++) {
        auto v = vecOfVecs->at(i);
        for (int j = 0; j < LARGE_VECTOR_SIZE; j++) {
            v[j] = normdist(mt);
        }
        std::partial_sum(v.begin(), v.end(), v.begin());
        vecOfVecs->at(i) = v;
    }
    
    gp << "set title 'Graph of two random lines'\n";
    gp << "plot '-' with lines title 'v0',"
        << "'-' with lines title 'v1',"
        << "'-' with lines title 'v2',"
        << "'-' with lines title 'v3',"
        << "'-' with lines title 'v4',"
        << "'-' with lines title 'v5',"
        << "'-' with lines title 'v6',"
        << "'-' with lines title 'v7',"
        << "'-' with lines title 'v8',"
        << "'-' with lines title 'v9'\n";

    for (int i = 0; i < 10; i++) {
        gp.send(vecOfVecs->at(i));
    }

    std::cin.get();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
