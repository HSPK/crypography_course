/**
 * author: whxway
 * email: whxway@qq.com
 * date: 2021-10-15
*/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "aes_ni.h"


int main(int argc, char const *argv[])
{
    uint8_t enc_key[]    = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    __m128i key_schedule[20];
    aes128_load_key(enc_key,key_schedule);

    clock_t start, end;
    double diff;
    FILE *fp = fopen("test.txt", "rb");
    long file_size;
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    uint8_t *plain = (uint8_t *)malloc(file_size);
    uint8_t *computed_cipher = (uint8_t *)malloc(file_size);
    uint8_t *computed_plain = (uint8_t *)malloc(file_size);

    size_t read_size = fread(plain, 1, file_size, fp);
    if (read_size != file_size) {
        printf("not enough memory! read: %I64d\n", read_size);
        exit(-1);
    }
    // test encrypt speed
    start = clock();
    for (long i = 0; i < file_size; i += 16) {
        aes128_enc(key_schedule, plain + i, computed_cipher + i);
    }
    end = clock();
    diff = end - start;
	printf ("encrypt time: %lf, speed: %.2f Mb/s\n", 
                diff / CLOCKS_PER_SEC, 
                file_size / diff * CLOCKS_PER_SEC / 1024 / 1024);
    // test decrypt speed
    start = clock();  
    for (long i = 0; i < file_size; i += 16) {
        aes128_dec(key_schedule, computed_cipher + i, computed_plain + i);
    }
    end = clock();
    diff = end - start;
    printf ("decrypt time: %lf, speed: %.2f Mb/s\n", 
                diff / CLOCKS_PER_SEC, 
                file_size / diff * CLOCKS_PER_SEC / 1024 / 1024);
    
    free(plain);
    free(computed_cipher);
    free(computed_plain);
    return 0;
}
