#include <stdio.h>
#include <stdlib.h>

int power2bits (int n, int min_bits, int max_bits)
{
    int max = (1 << max_bits) - 1;
    if (n > max)
        return max_bits;

    /* count the bits in n. */
    int i = 0;
    int tmp = n;
    do
    {
        tmp >>= 1;
        ++i;

    }
    while (tmp != 0);

    if (i <= min_bits)
        return min_bits;

    /* Which is nearest? */
    int a = (1 << i) - n;
    int b = (1 << (i - 1)) - n;
    if (b < 0)
        b = -b;
    if (b < a)
        return i - 1;
    return i;
}


int main(int, char *[])
{
    int count = power2bits(1024, 3, 12); // 3 - 12 ==>  8-4096
    fprintf(stderr, "power2bits(1025, 3, 12) -> %d\n", power2bits(1025, 3, 12));
    fprintf(stderr, "power2bits(4096, 3, 12) -> %d\n", power2bits(4096, 3, 12));
    fprintf(stderr, "power2bits(20, 1, 20) -> %d\n", power2bits(20, 1, 20));

    fprintf(stderr, "1 << 4: %d, 1 << 10: %d\n", 1 << 4, 1 << 10);

    return 0;
}
