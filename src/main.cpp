#include <main.h>
int main()
{
    Brain test(2, 3, 1);

    test
        .logstats()
        .logstate()
    ;
}
