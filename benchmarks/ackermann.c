#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

int64_t ackermann(int64_t m, int64_t n) {
    if (m == 0) {
        return n + 1;
    }
    if (n == 0) {
        return ackermann(m - 1, 1);
    }
    return ackermann(m - 1, ackermann(m, n - 1));
}

int main(void) {
    printf("%" PRId64 "\n", ackermann(3, 11));
    return 0;
}
