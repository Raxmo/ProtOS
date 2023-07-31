#pragma once

#include <random>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
class Brain
{
private:
    double dt;
    double tick;

    double* input;      // These are the 3 different vetors that form the state vector
    double* compute;    // they are split up here for easier implimentation
    double* output;     // Mathematically, they could be represented by a singular vector.

    double** itcs[3];   // These are the atricies for the governing polynomial.
    double** itos[3];   // This version uses a degree 3 polinomial.
    double** ctcs[3];   // A degree 5 polinomial will likely be better, but will be more costly
    double** ctos[3];   // This polinomial is not a true polynomial (as will be evident in the .cpp file) but has a similar form

    double* itcb;       // These are the contant terms in the polynomial
    double* itob;       // The full polynomial has the input, compute, and output vectors concatonated into one.
    double* ctcb;       // it isn't really neccessary to have them seperate, but it helps with implimentation
    double* ctob;       // The same is said about the matricies, here they are broken up into 4 seperate matricies.
public:
    int numbin;
    int numbout;
    int numbcomp;
private:
    void governer();

public:
    Brain(int nin, int ncomp, int nout);
    ~Brain();
    void test();
    Brain printstats();
    Brain logstats();
    Brain logstate();
    Brain logmats();
};
