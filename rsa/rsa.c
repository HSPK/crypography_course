#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#define PRIME_LENGTH 512

unsigned long GMP_SEED = 233;
typedef struct
{
	mpz_t e;
	mpz_t N;
    mpz_t d;
} rsa_key;

void init_random_state(gmp_randstate_t state) {
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, GMP_SEED);
}

void get_prime(mpz_t p, gmp_randstate_t state) {
    mpz_rrandomb(p, state, PRIME_LENGTH);
    while (!(mpz_millerrabin(p, PRIME_LENGTH))) {
        gmp_randclear(state);
        GMP_SEED++;
        init_random_state(state);
        mpz_rrandomb(p, state, PRIME_LENGTH);
    }
    gmp_randclear(state);
    GMP_SEED++;
}

void key_generation(rsa_key *key) {
    gmp_randstate_t state;
    mpz_set_ui(key->e, 65537);
    mpz_t p, q;
    mpz_inits(p, q, NULL);
    init_random_state(state);
    get_prime(p, state);
    init_random_state(state);
    get_prime(q, state);
    mpz_mul(key->N, p, q);
    mpz_t p_minus1, q_minus1, phi_N;
    mpz_inits(p_minus1, q_minus1, phi_N, NULL);
    mpz_sub_ui(p_minus1, p, 1);
    mpz_sub_ui(q_minus1, q, 1);
    mpz_mul(phi_N, p_minus1, q_minus1);
    mpz_clears(p, q, p_minus1, q_minus1, NULL);
    mpz_invert(key->d, key->e, phi_N);
}

void rsa_encode(mpz_t encode, unsigned char encode_arr[]) {
    mpz_export(encode_arr, NULL, 1, sizeof(encode_arr[0]), 0, 0, encode);
}

void rsa_decode(mpz_t decode, unsigned char decode_arr[]) {
    mpz_import(decode, 128, 1, sizeof(decode_arr[0]), 0, 0, decode_arr);
}

void rsa_encrypt(mpz_t cipher_text, mpz_t plain_text, rsa_key *key) {
    mpz_powm(cipher_text, plain_text, key->e, key->N);
}

void rsa_decrypt(mpz_t decrypt_text, mpz_t cipher_text, rsa_key *key) {
    mpz_powm(decrypt_text, cipher_text, key->d, key->N);
}

void test() {
    rsa_key key;
    mpz_t plain_text, cipher_text, decrypt_text;
    mpz_inits(key.e, key.N, key.d, NULL);
    mpz_inits(plain_text, cipher_text, decrypt_text, NULL);
    unsigned char plain_text_arr[128] = "hello world!";
    unsigned char decrypt_text_arr[128]; 

    gmp_printf("generating key...\n");
    key_generation(&key);
    
    gmp_printf("e : %Zd\nd : %Zd\nN : %Zd\n", key.e, key.d, key.N);
    gmp_printf("encrypting...\n");
    rsa_decode(plain_text, plain_text_arr);
    gmp_printf("plain text : %s\n", plain_text_arr);
    gmp_printf("plain text val : %Zd\n", plain_text);
    rsa_encrypt(cipher_text, plain_text, &key);
    gmp_printf("decrypting...\n");
    rsa_decrypt(decrypt_text, cipher_text, &key);
    rsa_encode(decrypt_text, decrypt_text_arr);
    gmp_printf("decrypt text : %s\n", decrypt_text_arr);
    gmp_printf("decrypt text val : %Zd\n", decrypt_text);
}

int main(int argc, char const *argv[])
{
    test();
    return 0;
}
