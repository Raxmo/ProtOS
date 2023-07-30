#pragma once

#include <random>
#include <chrono>
#include <iostream>
class Brain
{
private:
    double* input;      // These are the 3 different vetors that form the state vector
    double* compute;    // they are split up here for easier implimentation
    double* output;     // Mathematically, they could be represented by a singular vector.

    double** itcs[2];   // These are the atricies for the governing polynomial.
    double** itos[2];   // This version uses a degree 3 polinomial.
    double** ctcs[2];   // A degree 5 polinomial will likely be better, but will be more costly
    double** ctos[2];   // This polinomial is not a true polynomial (as will be evident in the .cpp file) but has a similar form

    double* itcb;       // These are the contant terms in the polynomial
    double* itob;       // The full polynomial has the input, compute, and output vectors concatonated into one.
    double* ctcb;       // it isn't really neccessary to have them seperate, but it helps with implimentation
    double* ctob;       // The same is said about the matricies, here they are broken up into 4 seperate matricies.

    double* itc3;       // These are the VECTOR coeficients or the highest term of the polynomial
    double* ito3;       // These are here to make sure that the state vector stays within reasonable values
    double* ctc3;       // The highest order needs to be odd in order to have a global attractor
    double* cto3;       // This polynomial could be applied to acceleration ather than velocity, but that will be for future testing
public:
    int numbin;
    int numbout;
    int numbcomp;
private:

public:
    Brain(int nin, int ncomp, int nout);
    ~Brain();
    void test();
};
