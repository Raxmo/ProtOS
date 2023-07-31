#include <brain.h>
Brain Brain::study(double** indat, double** outdat, int numbentry)
{
    std::cout << "setting up perams...\n";

    double plasticity = 0.99;
    double decay = 0.001;
    int params =
        numbcomp +
        numbout +
        3 * numbin * numbcomp +
        3 * numbcomp * numbcomp +
        3 * numbin * numbout +
        3 * numbcomp * numbout
    ;

    std::cout << "Setting up random number generator\n";

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<>  rng(0, params - 1);
    std::uniform_real_distribution<> z1(0.0, 1.0);
    std::uniform_int_distribution<>  ent(0, numbentry - 1);

    /* Monte carlo optomizer
     * 1) test with current state
     * 2) make a random change
     * 3) test again with new state
     * 4) if new is better, keep it with probability plasticity
     * 5) if old is better, keep it with probability plasticity
     * 6) decay plasticity by decay rate
     * 7) go to step 1 if plasticity > 0.01
     */

    std::cout << "Starting core loop\n";

    for(;plasticity > 0.01; plasticity *= 1.0 - decay)
    {

        int entry = ent(mt);

        // set input to the test data

        for(int r = 0; r < numbin; r++)
        {
            input[r] = indat[entry][r];
        }

        step();
        double olddist = 0.0;
        for(int r = 0; r < numbout; r++)
        {
            olddist += (output[r] - outdat[entry][r]) * (output[r] - outdat[entry][r]);
        }

        //----------------------------------------------------------------------------//

        int i = rng(mt);

        double* target = nullptr;
        double oldval = 0.0;

        //---------------------------------------------------------------------------//

        if(i < numbcomp && target == nullptr)
        {
            oldval = xtcb[i];
            target = &xtcb[i];
        }
        else
        {
            i -= numbcomp;
        }

        if(i < numbout && target == nullptr)
        {
            oldval = xtob[i];
            target = &xtob[i];
        }
        else
        {
            i -= numbout;
        }

        if(i < 3 * numbin * numbcomp && target == nullptr)
        {
            int c = i % numbin;
            int r = (i / numbin) % numbcomp;
            int d = i / (numbin * numbcomp);

            oldval = itcs[d][r][c];
            target = &itcs[d][r][c];
        }
        else
        {
            i -= 3 * numbin * numbcomp;
        }

        if(i < 3 * numbcomp * numbcomp && target == nullptr)
        {
            int c = i % numbcomp;
            int r = (i / numbcomp) % numbcomp;
            int d = i / (numbcomp * numbcomp);

            oldval = ctcs[d][r][c];
            target = &ctcs[d][r][c];
        }
        else
        {
            i -= 3 * numbcomp * numbcomp;
        }

        if(i < 3 * numbin * numbout && target == nullptr)
        {
            int c = i % numbin;
            int r = (i / numbin) % numbout;
            int d = i / (numbin * numbout);

            oldval = itos[d][r][c];
            target = &itos[d][r][c];
        }
        else
        {
            i -= 3 * numbin * numbout;
        }

        if(i < 3 * numbcomp * numbout && target == nullptr)
        {
            int c = i % numbcomp;
            int r = (i / numbcomp) % numbout;
            int d = i / (numbcomp * numbout);

            oldval = ctos[d][r][c];
            target = &ctos[d][r][c];
        }
        else
        {
            i -= 3 * numbcomp * numbout;
        }

        //----------------------------------------------------------------------------------//

        double choice = z1(mt);
        double mult = choice < 1.0 / 3.0 ? -1.0 : choice < 2.0 / 3.0 ? 1.01 : 1.0 / 1.01;

        *target *= mult;

        //----------------------------------------------------------------------------------//

        step();
        double newdist = 0.0;
        for(int r = 0; r < numbout; r++)
        {
            newdist += (output[r] - outdat[entry][r]) * (output[r] - outdat[entry][r]);
        }

        //----------------------------------------------------------------------------------//

        if(newdist < olddist)
        {
            if(z1(mt) < plasticity)
            {
                *target = *target;
            }
            else
            {
                *target = oldval;
            }
        }
        else
        {
            if(z1(mt) < plasticity)
            {
                *target = oldval;
            }
            else
            {
                *target = *target;
            }
        }
    }

    return *this;
}
Brain Brain::step()
{
    for(double t = 0.0; t < tick; t += dt)
    {
        governer();
    }

    return *this;
}

Brain Brain::setinput(int r, double v)
{
    input[r] = v;

    return *this;
}
Brain Brain::readoutput()
{
    std::cout << "Output state:\n";
    for(int r = 0; r < numbout; r++)
    {
        std::cout << "[" << r << "]-> " << output[r] << "\n";
    }

    return *this;
}
Brain Brain::savestate()
{
    std::ofstream ofile("data/state.dat", std::ios::binary);
    if(!ofile){std::cout << "Failed to save state!\n"; return *this;}

    for(int r = 0; r < numbcomp; r++)
    {
        ofile.write(reinterpret_cast<char *>(&compute[r]), sizeof(compute[r]));
    }

    ofile.close();

    return *this;
}
Brain Brain::savemats()
{
    std::ofstream ofile("data/mats.dat", std::ios::binary);
    if(!ofile){std::cout << "Failed to save matricies!\n"; return *this;}

    for(int d = 0; d < 3; d++)
    {
        for(int r = 0; r < numbcomp; r++)
        {
            for(int c = 0; c < numbin; c++)
            {
                ofile.write(reinterpret_cast<char *>(&itcs[d][r][c]), sizeof(itcs[d][r][c]));
            }

            for(int c = 0; c < numbcomp; c++)
            {
                ofile.write(reinterpret_cast<char *>(&ctcs[d][r][c]), sizeof(ctcs[d][r][c]));
            }
        }
        for(int r = 0; r < numbout; r++)
        {
            for(int c = 0; c < numbin; c++)
            {
                ofile.write(reinterpret_cast<char *>(&itos[d][r][c]), sizeof(itos[d][r][c]));
            }
            for(int c = 0; c < numbcomp; c++)
            {
                ofile.write(reinterpret_cast<char *>(&ctos[d][r][c]), sizeof(ctos[d][r][c]));
            }
        }
    }

    ofile.close();

    return *this;
}
Brain Brain::readstate()
{
    std::ifstream ifile("data/state.dat", std::ios::binary);
    if(!ifile){std::cout << "Failed to open state!\n"; return *this;}

    for(int r = 0; r < numbcomp; r++)
    {
        ifile.read(reinterpret_cast<char *>(&compute[r]), sizeof(compute[r]));
    }

    ifile.close();

    return *this;
}
Brain Brain::readmats()
{
    std::ifstream ifile("data/mats.dat", std::ios::binary);
    if(!ifile){std::cout << "failed to read matricies!\n"; return *this;}

    for(int d = 0; d < 3; d++)
    {
        for(int r = 0; r < numbcomp; r++)
        {
            for(int c = 0; c < numbin; c++)
            {
                ifile.read(reinterpret_cast<char *>(&itcs[d][r][c]), sizeof(itcs[d][r][c]));
            }

            for(int c = 0; c < numbcomp; c++)
            {
                ifile.read(reinterpret_cast<char *>(&ctcs[d][r][c]), sizeof(ctcs[d][r][c]));
            }
        }
        for(int r = 0; r < numbout; r++)
        {
            for(int c = 0; c < numbin; c++)
            {
                ifile.read(reinterpret_cast<char *>(&itos[d][r][c]), sizeof(itos[d][r][c]));
            }
            for(int c = 0; c < numbcomp; c++)
            {
                ifile.read(reinterpret_cast<char *>(&ctos[d][r][c]), sizeof(ctos[d][r][c]));
            }
        }
    }

    return *this;
}
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
        {ofile.open("logs/Stats.md", std::ios::out | std::ios::trunc);

            std::stringstream buff;

            buff << "# Brain Stats:\n";
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
        {ofile.open("logs/State.md", std::ios::out | std::ios::trunc);

            ofile << "# Brain State:\n";
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
    {std::fstream ofile;
        {ofile.open("logs/Mats.md", std::ios::out | std::ios::trunc);

            ofile << "# Brain Matricies:\n";
            ofile << "-----\n";

            //------------------------------------------------------------------------------------------------------------//

            ofile << "## Bias Vectors:\n";
            ofile << "| Compute | Output |\n";
            ofile << "|:-------:|:------:|\n";
            for (int r = 0; r < std::max(numbcomp, numbout); r++)
            {
                ofile << "| ";
                ofile << (r < numbcomp ? std::to_string(xtcb[r]) : " ") << " | ";
                ofile << (r < numbout ? std::to_string(xtob[r]) : " ") << " |\n";
            }
            ofile << "\n";
            ofile << "-----\n";

            //---------------------------------------------------------------------------------------------------------//

            for(int d = 0; d < 3; d++)
            {
                ofile << "## Input to Compute Matrix " << d + 1 << ":\n";
                ofile << "|";
                for(int c = 0; c < numbin; c++)
                {
                    ofile << " |";
                }
                ofile << "\n";
                ofile << "|";
                for(int c = 0; c < numbin; c++)
                {
                    ofile << ":---:|";
                }
                ofile << "\n";
                for(int r = 0; r < numbcomp; r++)
                {
                    ofile << "|";
                    for(int c = 0; c < numbin; c++)
                    {
                        ofile << itcs[d][r][c] << " |";
                    }
                    ofile << "\n";
                }
                ofile << "\n";
            }
            ofile << "\n";
            ofile << "-----\n";

            //--------------------------------------------------------------------------------------------------------------//

            for(int d = 0; d < 3; d++)
            {
                ofile << "## Compute to Compute Matrix " << d + 1 << ":\n";
                ofile << "|";
                for(int c = 0; c < numbcomp; c++)
                {
                    ofile << " |";
                }
                ofile << "\n";
                ofile << "|";
                for (int c = 0; c < numbcomp; c++)
                {
                    ofile << ":---:|";
                }
                ofile << "\n";
                for(int r = 0; r < numbcomp; r++)
                {
                    ofile << "|";
                    for(int c = 0; c < numbcomp; c++)
                    {
                        ofile << ctcs[d][r][c] << " |";
                    }
                    ofile << "\n";
                }
                ofile << "\n";
            }
            ofile << "\n";
            ofile << "-----\n";

            //-----------------------------------------------------------------------------------------------------------------//

            for(int d = 0; d < 3; d++)
            {
                ofile << "## Input to Output Matrix " << d + 1 << ":\n";
                ofile << "|";
                for(int c = 0; c < numbin; c++)
                {
                    ofile << " |";
                }
                ofile << "\n";
                ofile << "|";
                for (int c = 0; c < numbin; c++)
                {
                    ofile << ":-----:|";
                }
                ofile << "\n";
                for(int r = 0; r < numbout; r++)
                {
                    ofile << "|";
                    for(int c = 0; c < numbin; c++)
                    {
                        ofile << itos[d][r][c] << " |";
                    }
                    ofile << "\n";
                }
                ofile << "\n";
            }
            ofile << "\n";
            ofile << "-----\n";

            //---------------------------------------------------------------------------------------------------------------//

            for(int d = 0; d < 3; d++)
            {
                ofile << "## Compute to Output Matrix " << d + 1 << ":\n";
                ofile << "|";
                for(int c = 0; c < numbcomp; c++)
                {
                    ofile << " |";
                }
                ofile << "\n";
                ofile << "|";
                for(int c = 0; c < numbcomp; c++)
                {
                    ofile << ":-----:|";
                }
                ofile << "\n";
                for(int r = 0; r < numbout; r++)
                {
                    ofile << "|";
                    for(int c = 0; c < numbcomp; c++)
                    {
                        ofile << ctos[d][r][c] << " |";
                    }
                    ofile << "\n";
                }
                ofile << "\n";
            }
            ofile << "\n";
            ofile << "-----\n";

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
        cv[r] = xtcb[r];

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
        ov[r] = xtob[r];

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
    std::uniform_real_distribution<> dist(-1, 1);

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
                ctos[p][r][c] = p == 2 ? dist(e2) / 2.0 - 0.5 : dist(e2);
            }
        }
    }

    // Hieghest order co-efficients are negative in order to maintain global attractor & the 0th order co-efficients
    xtcb = new double[ncomp];
    for(int v = 0; v < ncomp; v++)
    {
        xtcb[v] = dist(e2);
    }

    xtob = new double[nout];
    for(int v = 0; v < nout; v++)
    {
        xtob[v] = dist(e2);
    }
};
