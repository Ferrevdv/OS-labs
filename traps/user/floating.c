#include "user.h"

double get_sum()
{
    double sum = 0;

    for (double d = 1.23; d < 100; d *= 1.9)
    {
        sum += d;
        sleep(1);
    }

    return sum;
}

int main()
{
    double sum = 0;

    if (fork() != 0)
    {
        sum = get_sum();
        wait(0);
    }
    else
    {
        sum = get_sum();
    }

    printf("Sum: %d\n", (int)sum);

    return 0;
}
