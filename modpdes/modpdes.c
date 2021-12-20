#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>
#define ROUNDS 16
#define PRIME_LENGTH 64
unsigned long GMP_SEED = 233;

typedef struct {
    mpz_t p;
    mpz_t roundkey[ROUNDS];
} des_key;

void des_encrypt(mpz_t cipher, mpz_t plain, des_key *key) {
    mpz_t left, right, tmp;
    mpz_inits(left, right, tmp, NULL);
    mpz_divmod(left, right, plain, key->p);
    for (int round = 0; round < ROUNDS; round++) {
        if (mpz_cmp_ui(right, 0) != 0)
            mpz_invert(tmp, right, key->p);
        else
            mpz_set_ui(tmp, 0);
        mpz_add(tmp, tmp, key->roundkey[round]);
        mpz_mod(tmp, tmp, key->p);
        mpz_add(left, tmp, left);
        mpz_mod(left, left, key->p);

        if (round != ROUNDS - 1)
            mpz_swap(left, right);
    }
    mpz_mul(cipher, left, key->p);
    mpz_add(cipher, cipher, right);
}

void des_decrypt(mpz_t decrypt, mpz_t cipher, des_key *key) {
    mpz_t left, right, tmp;
    mpz_inits(left, right, tmp, NULL);
    mpz_divmod(left, right, cipher, key->p);
    for (int round = 0; round < ROUNDS; round++) {
        if (mpz_cmp_ui(right, 0) != 0)
            mpz_invert(tmp, right, key->p);
        else
            mpz_set_ui(tmp, 0);
        mpz_add(tmp, tmp, key->roundkey[ROUNDS - round - 1]);
        mpz_mod(tmp, tmp, key->p);
        mpz_sub(left, left, tmp);
        mpz_mod(left, left, key->p);

        if (round != ROUNDS - 1)
            mpz_swap(left, right);
    }
    mpz_mul(decrypt, left, key->p);
    mpz_add(decrypt, decrypt, right);
}

void init_random_state(gmp_randstate_t state) {
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, GMP_SEED);
}

void init_des_key(des_key *key, mpz_t p, gmp_randstate_t state) {
    mpz_init(key->p);
    for (int i = 0; i < ROUNDS; i++) mpz_init(key->roundkey[i]);
    mpz_set(key->p, p);
    for (int i = 0; i < ROUNDS; i++) {
        init_random_state(state);
        GMP_SEED++;
        mpz_urandomm(key->roundkey[i], state, key->p);
    }
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

void speed_test() {
    mpz_t cipher, plain, decrypt;
    mpz_t p;
    gmp_randstate_t state;
    des_key key;
    mpz_inits(cipher, plain, decrypt, p, NULL);
    init_random_state(state);
    get_prime(p, state);
    init_des_key(&key, p, state);
    mpz_set_ui(plain, 151654);
    int loops = 10000;
    clock_t s = clock();
    for (int i = 0; i < loops; i++)
        des_encrypt(cipher, plain, &key);
    clock_t e = clock();
    double duration = (double)(e - s) / CLOCKS_PER_SEC;
    printf("encryt duration: %f, speed: %f Mb/s\n", duration, PRIME_LENGTH * loops / duration);
}

void test() {
    mpz_t cipher, plain, decrypt;
    mpz_t p;
    gmp_randstate_t state;
    des_key key;
    mpz_inits(cipher, plain, decrypt, p, NULL);
    printf("generating prime p (0 < p < 2^1024)...\n");
    init_random_state(state);
    get_prime(p, state);
    gmp_printf("p: %Zd\n", p);
    gmp_printf("generating des round key...\n");
    init_des_key(&key, p, state);
    gmp_printf("generate successful\n");
    mpz_set_ui(plain, 151654);
    gmp_printf("plain  : %Zd\n", plain);
    des_encrypt(cipher, plain, &key);
    gmp_printf("cipher : %Zd\n", cipher);
    des_decrypt(decrypt, cipher, &key);
    gmp_printf("decrypt: %Zd\n", decrypt);
}

int main(int argc, char const *argv[])
{
    test();
    speed_test();
    return 0;
}
