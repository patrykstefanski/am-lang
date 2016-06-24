-- FIXME: Will overflow the stack.

function ackermann (m, n)
    if m == 0 then
        return n + 1
    end
    if n == 0 then
        return ackermann(m - 1, 1)
    end
    return ackermann(m - 1, ackermann(m, n - 1))
end

print(ackermann(3, 11))
