#include <stdio.h>
#include <string.h>
#include "my_crc32.h"

namespace utils
{

unsigned int POLYNOMIAL = 0xEDB88320 ;
int have_table = 0 ;
unsigned int table[256] ;


void make_table()
{
    int i, j;
    have_table = 1 ;
    for (i = 0 ; i < 256 ; i++)
        for (j = 0, table[i] = i ; j < 8 ; j++)
            table[i] = (table[i]>>1)^((table[i]&1)?POLYNOMIAL:0) ;
}


unsigned int crc32(char *buff, unsigned int len, unsigned int crc)
{
    if (!have_table) make_table() ;
    crc = ~crc;
    for (int i = 0; i < len; i++)
        crc = (crc >> 8) ^ table[(crc ^ buff[i]) & 0xff];
    return ~crc;
}

}

/*
int main ()
{
    char s[] = "aaaaaa";
    printf("%08Xh\n", crc32(0, s, strlen(s)));
    return 0 ;
}
*/

