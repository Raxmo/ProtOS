#include <brain.h>
void Brain::test()
{
    std::cout << "Hello Test Brain!" << std::endl;
    std::cout << "This brain has " << numbin << " inputs." << std::endl;
    std::cout << "As well as " << numbcomp << " computation neurons." << std::endl;
    std::cout << "And " << numbout << " outputs." << std::endl;
};

Brain::~Brain(){};

Brain::Brain(int nin, int ncomp, int nout)
{
    // basic variable set-up
    numbin = nin;
    numbcomp = ncomp;
    numbout = nout;

    input = new double[nin];
    compute = new double[ncomp];
    output = new double[nout];

    // RNG set-up
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0, 1);

    // Polynomial co-efficients set-up
    for (int p = 0; p < 2; p++)
    {
        itcs[p] = new double*[ncomp];
        ctcs[p] = new double*[ncomp];

        for(int r = 0; r < ncomp; r++)
        {
            itcs[p][r] = new double[nin];
            ctcs[p][r] = new double[ncomp];

            for (int c = 0; c < nin; c++)
            {
                itcs[p][r][c] = dist(e2);
            }
            for (int c = 0; c < ncomp; c++)
            {
                ctcs[p][r][c] = dist(e2);
            }
        }

        itos[p] = new double*[nout];
        ctos[p] = new double*[nout];

        for (int r = 0; r < nout; r++)
        {
            itos[p][r] = new double[nin];
            ctos[p][r] = new double[ncomp];

            for(int c = 0; c < nin; c++)
            {
                itos[p][r][c] = dist(e2);
            }
            for(int c = 0; c < ncomp; c++)
            {
                ctcs[p][r][c] = dist(e2);
            }
        }
    }

    // Hieghest order co-efficients are negative in order to maintain global attractor
    itc3 = new double[ncomp];
    ctc3 = new double[ncomp];
    for(int v = 0; v < ncomp; v++)
    {
        itc3[v] = dist(e2) / 2.0 - 0.5;
        ctc3[v] = dist(e2) / 2.0 - 0.5;
    }

    ito3 = new double[nout];
    cto3 = new double[nout];
    for(int v = 0; v < nout; v++)
    {
        ito3[v] = dist(e2) / 2.0 - 0.5;
        ito3[v] = dist(e2) / 2.0 - 0.5;
    }
};
