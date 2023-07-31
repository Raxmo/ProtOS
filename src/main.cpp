#include <main.h>
int main()
{
    Brain test(2, 3, 1);

    double** indat;
    double** outdat;

    indat  = new double*[4];
    outdat = new double*[4];

    for(int i = 0; i < 4; i++)
    {
        indat[i] = new double[2];
        outdat[i] = new double[1];
    }

    indat[0][0] = 0.0;
    indat[0][1] = 0.0;
    outdat[0][0] = 0.0;

    indat[1][0] = 0.0;
    indat[1][1] = 1.0;
    outdat[1][0] = 1.0;

    indat[2][0] = 1.0;
    indat[2][1] = 0.0;
    outdat[2][0] = 1.0;

    indat[3][0] = 1.0;
    indat[3][1] = 1.0;
    outdat[3][0] = 0.0;

    test
        .study(indat, outdat, 4)
        .logstats()
        .logstate()
        .logmats()
    ;
}
