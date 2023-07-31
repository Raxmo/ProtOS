#include <brain.h>
Brain Brain::printstats()
{
    std::cout << "[INPUTS]--> " << numbin << "\n";
    std::cout << "[COMPUTE]-> " << numbcomp << "\n";
    std::cout << "[OUTPUT]--> " << numbout << "\n";

    return *this;
}

Brain Brain::logstats()
{
    {std::fstream ofile;
        {ofile.open("stats.md", std::ios::out | std::ios::trunc);

            std::stringstream buff;

            buff << "Brain Stats:\n";
            buff << "| Inputs | Computes | Outputs |\n";
            buff << "|:------:|:--------:|:-------:|\n";
            buff << "| " << numbin << " | " << numbcomp << " | " << numbout << " |";

            ofile << buff.str();

        ofile.close();}
    }

    return *this;
}

Brain Brain::logstate()
{
    {std::fstream ofile;
        {ofile.open("State.md", std::ios::out | std::ios::trunc);

            ofile << "Brain State:\n";
            ofile << "| Input | Compute | Output |\n";
            ofile << "|:-----:|:-------:|:------:|\n";

            for(int r = 0; r < std::max(std::max(numbin, numbcomp), numbout); r++)
            {
                ofile <<  "| " << (r < numbin   ? std::to_string(input[r])   : "---");
                ofile << " | " << (r < numbcomp ? std::to_string(compute[r]) : "---");
                ofile << " | " << (r < numbout  ? std::to_string(output[r])  : "---");
                ofile << " |\n";
            }

        ofile.close();}
    }

    return *this;
}

Brain Brain::logmats()
{
    {std::ftream ofile;
        {ofile.open("Mats.md", std::ios::out | std::ios::trunc);

            ofile << "Brain Matricies:\n";

        ofile.close();}
    }

    return *this;
}

void Brain::test()
{
    std::cout << "Hello Test Brain!" << std::endl;
    std::cout << "This brain has " << numbin << " inputs." << std::endl;
    std::cout << "As well as " << numbcomp << " computation neurons." << std::endl;
    std::cout << "And " << numbout << " outputs." << std::endl;
};

void Brain::governer() // third order polynomial
{
    double cv[numbcomp];
    double ov[numbout];

    for(int r = 0; r < numbcomp; r++)
    {
        cv[r] = itcb[r] + ctcb[r];

        for(int c = 0; c < numbin; c++)
        {
            cv[r] += itcs[0][r][c] * input[c];
            cv[r] += itcs[1][r][c] * input[c] * input[c];
            cv[r] += itcs[2][r][c] * input[c] * input[c] * input[c];
        }
        for(int c = 0; c < numbcomp; c++)
        {
            cv[r] += ctcs[0][r][c] * compute[c];
            cv[r] += ctcs[1][r][c] * compute[c] * compute[c];
            cv[r] += ctcs[2][r][c] * compute[c] * compute[c] * compute[c];
        }

        cv[r] *= dt;

        compute[r] += cv[r];
    }
    for(int r = 0; r < numbout; r++)
    {
        ov[r] = itob[r] + ctob[r];

        for(int c = 0; c < numbin; c++)
        {
            ov[r] += itos[0][r][c] * input[c];
            ov[r] += itos[1][r][c] * input[c] * input[c];
            ov[r] += itos[2][r][c] * input[c] * input[c] * input[c];
        }
        for(int c = 0; c < numbcomp; c++)
        {
            ov[r] += ctos[0][r][c] * compute[c];
            ov[r] += ctos[1][r][c] * compute[c] * compute[c];
            ov[r] += ctos[2][r][c] * compute[c] * compute[c] * compute[c];
        }

        ov[r] *= dt;

        output[r] += ov[r];
    }
}

Brain::~Brain(){};

Brain::Brain(int nin, int ncomp, int nout)
{
    tick = 1.0;
    dt = 0.01;

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
    for (int p = 0; p < 3; p++)
    {
        itcs[p] = new double*[ncomp];
        ctcs[p] = new double*[ncomp];

        for(int r = 0; r < ncomp; r++)
        {
            itcs[p][r] = new double[nin];
            ctcs[p][r] = new double[ncomp];

            for (int c = 0; c < nin; c++)
            {
                itcs[p][r][c] = p == 2 ? dist(e2) / 2.0 - 0.5 : dist(e2);
            }
            for (int c = 0; c < ncomp; c++)
            {
                ctcs[p][r][c] = p == 2 ? dist(e2) / 2.0 - 0.5 : dist(e2);
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
                itos[p][r][c] = p == 2 ? dist(e2) / 2.0 - 0.5 : dist(e2);
            }
            for(int c = 0; c < ncomp; c++)
            {
                ctcs[p][r][c] = p == 2 ? dist(e2) / 2.0 - 0.5 : dist(e2);
            }
        }
    }

    // Hieghest order co-efficients are negative in order to maintain global attractor & the 0th order co-efficients
    itcb = new double[ncomp];
    ctcb = new double[ncomp];
    for(int v = 0; v < ncomp; v++)
    {
        itcb[v] = dist(e2);
        ctcb[v] = dist(e2);
    }

    itob = new double[nout];
    ctob = new double[nout];
    for(int v = 0; v < nout; v++)
    {
        itob[v] = dist(e2);
        ctob[v] = dist(e2);
    }
};
