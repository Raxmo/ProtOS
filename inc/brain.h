#pragma once

#include <random>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cmath>

class Brain
{
private:
    double dt;
    double tick;

    double* input;
    double* compute;
    double* output;

    double** itcs[3];
    double** itos[3];
    double** ctcs[3];
    double** ctos[3];

    double* xtcb;
    double* xtob;
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
    Brain savestate();
    Brain savemats();
    Brain readstate();
    Brain readmats();
    Brain setinput(int r, double v);
    Brain readoutput();
    Brain step();
    Brain study(double** indat, double** outdat, int numbentry);
};
