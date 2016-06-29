function is_prime (n)
    if n < 2 then
        return 0
    end
    if n == 2 then
        return 1
    end
    local i = 2
    while i < n do
        if n % i == 0 then
            return 0
        end
        i = i + 1
    end
    return 1
end

print(is_prime(314606869))
