#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "kem.h"
#include "kex.h"
#include "params.h"
#include "indcpa.h"
#include "polyvec.h"
#include "poly.h"
//#include "cpucycles.h"
//#include "speed_print.h"
#include "fips202.h"
#include <string.h>
#include "fpga_sha.h"

int main(void)
{
    char input[800]="I love you";
    // gets(input);
    uint8_t *text;
    text=input;
    // uint8_t text[6] = {0xfd, 0x69, 0x9b, 0x12, 0xab, 0xac};
    uint8_t output1[32];
    sha3_256(output1,text,strlen(input));
    printf("SHA3-256 hashvalue is: ");
    for (uint8_t i=0; i<32; i++)
    {
        printf("%02x", output1[i]);
    }
    uint8_t output2[64];
    sha3_512(output2,text,strlen(input));
    printf("\nSHA3-512 hashvalue is: ");
    for (uint8_t i=0; i<64; i++)
    {
        printf("%02x", output2[i]);
    }
    hps_fpgainit();
    fpga_sha3_256(output3,text,strlen(input));
    printf("\nSHA3-256 hashvalue (FPGA) is: ");
    for (uint8_t i=0; i<32; i++)
    {
        printf("%02x", output3[i]);
    }
    hps_fpgarelease();
}

