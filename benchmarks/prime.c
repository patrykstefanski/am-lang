#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

int64_t is_prime(int64_t n) {
    int64_t i = 2;
    int64_t m = n / 2;
    while (i < m) {
        if (n % i == 0) {
            return 0;
        }
        i = i + 1;
    }
    return 1;
}

int main(void) {
    printf("%" PRId64 "\n", is_prime(314606869));
    return 0;
}
