function is_prime (n)
    local i = 2
    local m = n / 2
    while i < m do
        if n % i == 0 then
            return 0
        end
        i = i + 1
    end
    return 1
end

print(is_prime(314606869))
