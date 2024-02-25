#include <stdio.h>
#include <stdint.h>

int main(void)
{
    float f = 0x1p-127;
    fprintf(stderr, "%x\n", *(uint32_t *)&f);
}