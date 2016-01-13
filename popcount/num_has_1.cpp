#include <stdlib.h>
#include <stdio.h>

int main()
{
    int n = 0xF0;
    while(n>0)
    {
        int high = n&~(n-1);
        n = n ^ high;

        printf("hight %x, n %x\n", high, n);
    }

    return 0;
}
