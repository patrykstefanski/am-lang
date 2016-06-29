def is_prime(n):
    if n < 2:
        return 0
    if n == 2:
        return 2
    i = 2
    while i < n:
        if n % i == 0:
            return 0
        i = i + 1
    return 1

print(is_prime(314606869))
