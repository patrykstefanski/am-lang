#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <cstdint>
#include <cstdio>
#include "cxx_extensions.hpp"

// FIXME: Big endian.

class Instruction final {
public:
    enum Opcode : std::uint8_t {
        // Commutative binary instructions.
        ADDRR, // a <- b + c
        MULRR, // a <- b * c
        EQRR,  // a <- b == c
        NERR,  // a <- b != c
        ADDRI, // a <- b + $c
        MULRI, // a <- b * $c
        EQRI,  // a <- b == $c
        NERI,  // a <- b != $c
        // Noncommutative binary instructions.
        SUBRR, // a <- b - c
        DIVRR, // a <- b / c
        MODRR, // a <- b % c
        LTRR,  // a <- b < c
        LERR,  // a <- b <= c
        SUBRI, // a <- b - $c
        DIVRI, // a <- b / $c
        MODRI, // a <- b % $c
        LTRI,  // a <- b < $c
        LERI,  // a <- b <= $c
        SUBIR, // a <- $b - c
        DIVIR, // a <- $b / c
        MODIR, // a <- $b % c
        LTIR,  // a <- $b < c
        LEIR,  // a <- $b <= c
        // Unary instructions.
        NEG,   // a <- -b
        NOT,   // a <- !b
        // Move instructions.
        MOVI,  // a <- $d
        MOVR,  // a <- b
        // Jump instructions.
        JMP,   // goto d
        JT,    // if a != 0 goto $d
        JF,    // if a == 0 goto $d
        // Call/ret instructions.
        CALL,  // a <- a(a + 1, a + 2, ..., a + b)
        RETR,  // return a
        RETI,  // return $d
        // System instructions.
        EXIT,  // exit(a)
        IN,    // scanf("%lli", &a);
        OUT,   // printf("%lli\n", a);
    };

    constexpr Instruction() : data_(0) {}
    constexpr explicit Instruction(std::uint32_t data) : data_(data) {}
    constexpr Instruction(const Instruction&) = default;
    constexpr Instruction& operator=(const Instruction&) = default;

    constexpr Opcode opcode() const {
        return static_cast<Opcode>(data_ & 0xff);
    }

    constexpr void set_opcode(Opcode opcode) {
        reinterpret_cast<std::uint8_t*>(&data_)[0] =
            static_cast<std::uint8_t>(opcode);
    }

    constexpr std::uint8_t a() const {
        return data_ >> 8;
    }

    constexpr void set_a(std::uint8_t a) {
        reinterpret_cast<std::uint8_t*>(&data_)[1] = a;
    }

    constexpr std::uint8_t b() const {
        return data_ >> 16;
    }

    constexpr void set_b(std::uint8_t b) {
        reinterpret_cast<std::uint8_t*>(&data_)[2] = b;
    }

    constexpr std::uint8_t c() const {
        return data_ >> 24;
    }

    constexpr void set_c(std::uint8_t c) {
        reinterpret_cast<std::uint8_t*>(&data_)[3] = c;
    }

    constexpr std::int16_t d() const {
        return data_ >> 16;
    }

    constexpr void set_d(std::int16_t d) {
        reinterpret_cast<std::int16_t*>(&data_)[1] = d;
    }

    constexpr std::uint32_t data() const {
        return data_;
    }

    constexpr void set_data(std::uint32_t data) {
        data_ = data;
    }

    constexpr static Instruction make_abc(Opcode opcode, std::uint8_t a,
            std::uint8_t b, std::uint8_t c) {
        Instruction instruction;
        instruction.set_opcode(opcode);
        instruction.set_a(a);
        instruction.set_b(b);
        instruction.set_c(c);
        return instruction;
    }

    constexpr static Instruction make_ad(Opcode opcode, std::uint8_t a,
            std::uint16_t d) {
        Instruction instruction;
        instruction.set_opcode(opcode);
        instruction.set_a(a);
        instruction.set_d(d);
        return instruction;
    }

    // Prints the instruction.
    COLD void print(std::FILE* file) const;

private:
    std::uint32_t data_;
};

#endif // !INSTRUCTION_HPP
