#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define DEFAULT_E 65537
typedef long long int s64;

int is_prime(s64 p) {
    for (int i = 2; i <= sqrt(p); i++) {
        if (p % i == 0) return 0;
    }
    return 1;
}

void get_prime_32bit(s64 *p) {
    srand(time(NULL));
    do {
        *p = rand();
    } while (!is_prime(*p));
}

void set_primes(s64 *p, s64 *q, s64 *N, s64 *phi_N) {
    get_prime_32bit(p);
    do {
        get_prime_32bit(q);
    } while (*p == *q);
    *N = (*p) * (*q);
    *phi_N = (*p - 1) * (*q - 1);
}

s64 exgcd(s64 a, s64 b, s64 *x, s64 *y)
{
    s64 xi_1, yi_1, xi_2, yi_2;
    xi_2 = 1, yi_2 = 0;
    xi_1 = 0, yi_1 = 1;
    *x = 0, *y = 1;
    s64 r = a % b;
    s64 q = a / b;
    while (r) {
        *x = xi_2 - q * xi_1;
        *y = yi_2 - q * yi_1;
        
        xi_2 = xi_1;
        yi_2 = yi_1;
 
        xi_1 = *x, yi_1 = *y;
        a = b;
        b = r;
        r = a % b;
        q = a / b;
    }
    return b;
}

s64 find_d(s64 e, s64 phi_N) {
    s64 x, y;
    exgcd(e, phi_N, &x, &y);
    x = (x % phi_N + phi_N) % phi_N;
    return x;
}

s64 gcd_s64(s64 a, s64 b) {
    int tmp;
    while (b) {
        tmp = b;
        b = a % b;
        a = tmp;
    }
    return a;
}

int is_e_valid(s64 e, s64 phi_N) {
    return gcd_s64(e, phi_N) == 1;
}

void key_generation(s64 *N, s64 *e, s64*d) {
    s64 p, q, phi_N;
    *e = DEFAULT_E;
    do {
	    set_primes(&p, &q, N, &phi_N);
    } while (!is_e_valid(*e, phi_N));
    *d = find_d(*e, phi_N);
}

s64 qpow(s64 a, s64 b, s64 p) {
  s64 ans = 1;
  a = (a % p + p) % p;
  for (; b; b >>= 1) {
    if (b & 1) ans = (a * ans) % p;
    a = (a * a) % p;
  }
  return ans;
}

s64 cipher_s64(s64 in, s64 e, s64 N) {
    return qpow(in, e, N);
}

void cipher_decipher(s64 *in, int len, s64 *out, s64 e, s64 N) {
    for (int i = 0; i < len; i++) {
        out[i] = cipher_s64(in[i], e, N);
    }
}

void test() {
    s64 e, d, N;
    s64 text[] = {1, 2, 3, 4, 5};
    s64 cipher_text[sizeof(text) / sizeof(s64)];
    s64 decipher_text[sizeof(text) / sizeof(s64)];
    int len = sizeof(text) / sizeof(s64);
    printf("key generating...\n");
    key_generation(&N, &e, &d);
    printf("e : %llu\nd : %llu\nN : %llu\n", e, d, N);
    printf("encrypting...\n");
    printf("text : ");
    for (int i = 0; i < len; i++) printf(" %x" + !i, text[i]);
    printf("\n");
    cipher_decipher(text, len, cipher_text, e, N);
    printf("cipher text : ");
    for (int i = 0; i < len; i++) printf(" %x" + !i, cipher_text[i]);
    printf("\n");
    printf("decrypting...\n");
    cipher_decipher(cipher_text, len, decipher_text, d, N);
    printf("decipher text : ");
    for (int i = 0; i < len; i++) printf(" %x" + !i, decipher_text[i]);
    printf("\n");
    if (memcmp(text, decipher_text, len * sizeof(s64))) {
        printf("failed, e : %llu, d : %llu, N : %llu\n", e, d, N);
    } else {
        printf("success\n");
    }
}

int main(int argc, char const *argv[]) {
    test();
    return 0;
}
