#include "interpreter.hpp"
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <vector>
#include "config.hpp"
#include "cxx_extensions.hpp"
#include "instruction.hpp"

namespace {

// Commutative binary instructions.

void interpret_addrr(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] + regs[ip->c()];
    ++ip;
}

void interpret_mulrr(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] * regs[ip->c()];
    ++ip;
}

void interpret_eqrr(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] == regs[ip->c()];
    ++ip;
}

void interpret_nerr(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] != regs[ip->c()];
    ++ip;
}

void interpret_addri(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] + static_cast<std::int8_t>(ip->c());
    ++ip;
}

void interpret_mulri(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] * static_cast<std::int8_t>(ip->c());
    ++ip;
}

void interpret_eqri(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] == static_cast<std::int8_t>(ip->c());
    ++ip;
}

void interpret_neri(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] != static_cast<std::int8_t>(ip->c());
    ++ip;
}

// Noncommutative binary instructions.

void interpret_subrr(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] - regs[ip->c()];
    ++ip;
}

void interpret_divrr(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] / regs[ip->c()];
    ++ip;
}

void interpret_modrr(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] + regs[ip->c()];
    ++ip;
}

void interpret_ltrr(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] < regs[ip->c()];
    ++ip;
}

void interpret_lerr(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] <= regs[ip->c()];
    ++ip;
}

void interpret_subri(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] - static_cast<std::int8_t>(ip->c());
    ++ip;
}

void interpret_divri(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] / static_cast<std::int8_t>(ip->c());
    ++ip;
}

void interpret_modri(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] + static_cast<std::int8_t>(ip->c());
    ++ip;
}

void interpret_ltri(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] < static_cast<std::int8_t>(ip->c());
    ++ip;
}

void interpret_leri(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()] <= static_cast<std::int8_t>(ip->c());
    ++ip;
}

void interpret_subir(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = static_cast<std::int8_t>(ip->b()) - regs[ip->c()];
    ++ip;
}

void interpret_divir(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = static_cast<std::int8_t>(ip->b()) / regs[ip->c()];
    ++ip;
}

void interpret_modir(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = static_cast<std::int8_t>(ip->b()) + regs[ip->c()];
    ++ip;
}

void interpret_ltir(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = static_cast<std::int8_t>(ip->b()) < regs[ip->c()];
    ++ip;
}

void interpret_leir(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = static_cast<std::int8_t>(ip->b()) <= regs[ip->c()];
    ++ip;
}

// Unary instructions.

void interpret_neg(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = -regs[ip->b()];
    ++ip;
}

void interpret_not(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = !regs[ip->b()];
    ++ip;
}

// Move instructions.

void interpret_movi(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = ip->d();
    ++ip;
}

void interpret_movr(const Instruction*& ip, std::int64_t* const& regs) {
    regs[ip->a()] = regs[ip->b()];
    ++ip;
}

// Jump instructions.

void interpret_jmp(const Instruction*& ip) {
    ip += ip->d() + 1;
}

void interpret_jt(const Instruction*& ip, const std::int64_t* const& regs) {
    if (regs[ip->a()] != 0) {
        ip += ip->d() + 1;
    } else {
        ++ip;
    }
}

void interpret_jf(const Instruction*& ip, const std::int64_t* const& regs) {
    if (regs[ip->a()] == 0) {
        ip += ip->d() + 1;
    } else {
        ++ip;
    }
}

// Call/ret instructions.

void interpret_call(const Instruction*& ip, std::int64_t*& regs) {
    std::int32_t a = ip->a();
    const auto* tmp = ip;
    ip += regs[a] + 1;
    regs[a] = reinterpret_cast<std::int64_t>(tmp);
    regs += a + 1;
}

void interpret_retr(const Instruction*& ip, std::int64_t*& regs) {
    std::int64_t ret = regs[ip->a()];
    ip = reinterpret_cast<Instruction*>(regs[-1]);
    regs[-1] = ret;
    regs -= ip->a() + 1;
    ++ip;
}

void interpret_reti(const Instruction*& ip, std::int64_t*& regs) {
    std::int64_t ret = ip->d();
    ip = reinterpret_cast<Instruction*>(regs[-1]);
    regs[-1] = ret;
    regs -= ip->a() + 1;
    ++ip;
}

// System instructions.

int interpret_exit(const Instruction* const& ip,
        const std::int64_t* const& regs) {
    return regs[ip->a()];
}

void interpret_in(const Instruction*& ip, std::int64_t* const& regs) {
    // Suppress the unused result warning.
    if (std::scanf("%" PRId64, &regs[ip->a()])) {}
    ++ip;
}

void interpret_out(const Instruction*& ip, const std::int64_t* const& regs) {
    std::printf("%" PRId64 "\n", regs[ip->a()]);
    ++ip;
}

}

#if !defined(INTERPRETER_MEMORY_SIZE)
#   define INTERPRETER_MEMORY_SIZE (1024 * 1024)
#endif

extern int trace_flag;

#ifndef NDEBUG
    #define TRACE if (trace_flag != 0) {                      \
        std::fprintf(stderr, "%08zu ", ip - bytecode.data()); \
        ip->print(stderr);                                    \
        std::fputc('\n', stderr);                             \
    }
#else
    #define TRACE
#endif

#if defined(INTERPRETER_REPLICATE_SWITCH)

#define NEXT do {                                    \
    TRACE;                                           \
    switch (ip->opcode()) {                          \
    /* Commutative binary instructions. */           \
    case Instruction::ADDRR: goto instruction_addrr; \
    case Instruction::MULRR: goto instruction_mulrr; \
    case Instruction::EQRR: goto instruction_eqrr;   \
    case Instruction::NERR: goto instruction_nerr;   \
    case Instruction::ADDRI: goto instruction_addri; \
    case Instruction::MULRI: goto instruction_mulri; \
    case Instruction::EQRI: goto instruction_eqri;   \
    case Instruction::NERI: goto instruction_neri;   \
    /* Noncommutative binary instructions. */        \
    case Instruction::SUBRR: goto instruction_subrr; \
    case Instruction::DIVRR: goto instruction_divrr; \
    case Instruction::MODRR: goto instruction_modrr; \
    case Instruction::LTRR: goto instruction_ltrr;   \
    case Instruction::LERR: goto instruction_lerr;   \
    case Instruction::SUBRI: goto instruction_subri; \
    case Instruction::DIVRI: goto instruction_divri; \
    case Instruction::MODRI: goto instruction_modri; \
    case Instruction::LTRI: goto instruction_ltri;   \
    case Instruction::LERI: goto instruction_leri;   \
    case Instruction::SUBIR: goto instruction_subir; \
    case Instruction::DIVIR: goto instruction_divir; \
    case Instruction::MODIR: goto instruction_modir; \
    case Instruction::LTIR: goto instruction_ltir;   \
    case Instruction::LEIR: goto instruction_leir;   \
    /* Unary instructions. */                        \
    case Instruction::NEG: goto instruction_neg;     \
    case Instruction::NOT: goto instruction_not;     \
    /* Move instructions. */                         \
    case Instruction::MOVI: goto instruction_movi;   \
    case Instruction::MOVR: goto instruction_movr;   \
    /* Jump instructions. */                         \
    case Instruction::JMP: goto instruction_jmp;     \
    case Instruction::JT: goto instruction_jt;       \
    case Instruction::JF: goto instruction_jf;       \
    /* Call/ret instructions. */                     \
    case Instruction::CALL: goto instruction_call;   \
    case Instruction::RETR: goto instruction_retr;   \
    case Instruction::RETI: goto instruction_reti;   \
    /* System instructions. */                       \
    case Instruction::EXIT: goto instruction_exit;   \
    case Instruction::IN: goto instruction_in;       \
    case Instruction::OUT: goto instruction_out;     \
    default: UNREACHABLE();                          \
    }                                                \
} while (0)

int interpret(const std::vector<Instruction>& bytecode) {
    std::vector<std::int64_t> memory(
            INTERPRETER_MEMORY_SIZE / sizeof(std::int64_t));
    std::int64_t* regs = memory.data();
    const auto* ip = bytecode.data();
    NEXT;
// Commutative binary instructions.
instruction_addrr:
    interpret_addrr(ip, regs);
    NEXT;
instruction_mulrr:
    interpret_mulrr(ip, regs);
    NEXT;
instruction_eqrr:
    interpret_eqrr(ip, regs);
    NEXT;
instruction_nerr:
    interpret_nerr(ip, regs);
    NEXT;
instruction_addri:
    interpret_addri(ip, regs);
    NEXT;
instruction_mulri:
    interpret_mulri(ip, regs);
    NEXT;
instruction_eqri:
    interpret_eqri(ip, regs);
    NEXT;
instruction_neri:
    interpret_neri(ip, regs);
    NEXT;
// Noncommutative binary instructions.
instruction_subrr:
    interpret_subrr(ip, regs);
    NEXT;
instruction_divrr:
    interpret_divrr(ip, regs);
    NEXT;
instruction_modrr:
    interpret_modrr(ip, regs);
    NEXT;
instruction_ltrr:
    interpret_ltrr(ip, regs);
    NEXT;
instruction_lerr:
    interpret_lerr(ip, regs);
    NEXT;
instruction_subri:
    interpret_subri(ip, regs);
    NEXT;
instruction_divri:
    interpret_divri(ip, regs);
    NEXT;
instruction_modri:
    interpret_modri(ip, regs);
    NEXT;
instruction_ltri:
    interpret_ltri(ip, regs);
    NEXT;
instruction_leri:
    interpret_leri(ip, regs);
    NEXT;
instruction_subir:
    interpret_subir(ip, regs);
    NEXT;
instruction_divir:
    interpret_divir(ip, regs);
    NEXT;
instruction_modir:
    interpret_modir(ip, regs);
    NEXT;
instruction_ltir:
    interpret_ltir(ip, regs);
    NEXT;
instruction_leir:
    interpret_leir(ip, regs);
    NEXT;
// Unary instructions.
instruction_neg:
    interpret_neg(ip, regs);
    NEXT;
instruction_not:
    interpret_not(ip, regs);
    NEXT;
// Move instructions.
instruction_movi:
    interpret_movi(ip, regs);
    NEXT;
instruction_movr:
    interpret_movr(ip, regs);
    NEXT;
// Jump instructions.
instruction_jmp:
    interpret_jmp(ip);
    NEXT;
instruction_jt:
    interpret_jt(ip, regs);
    NEXT;
instruction_jf:
    interpret_jf(ip, regs);
    NEXT;
// Call/ret instructions.
instruction_call:
    interpret_call(ip, regs);
    NEXT;
instruction_retr:
    interpret_retr(ip, regs);
    NEXT;
instruction_reti:
    interpret_reti(ip, regs);
    NEXT;
// System instructions.
instruction_exit:
    return interpret_exit(ip, regs);
instruction_in:
    interpret_in(ip, regs);
    NEXT;
instruction_out:
    interpret_out(ip, regs);
    NEXT;
    UNREACHABLE();
    // Suppress the warning.
    return 0;
}

#else // !INTERPRETER_REPLICATE_SWITCH

int interpret(const std::vector<Instruction>& bytecode) {
    std::vector<std::int64_t> memory(
            INTERPRETER_MEMORY_SIZE / sizeof(std::int64_t));
    std::int64_t* regs = memory.data();
    const auto* ip = bytecode.data();
    for (;;) {
        TRACE;
        switch (ip->opcode()) {
        // Commutative binary instructions.
        case Instruction::ADDRR:
            interpret_addrr(ip, regs);
            break;
        case Instruction::MULRR:
            interpret_mulrr(ip, regs);
            break;
        case Instruction::EQRR:
            interpret_eqrr(ip, regs);
            break;
        case Instruction::NERR:
            interpret_nerr(ip, regs);
            break;
        case Instruction::ADDRI:
            interpret_addri(ip, regs);
            break;
        case Instruction::MULRI:
            interpret_mulri(ip, regs);
            break;
        case Instruction::EQRI:
            interpret_eqri(ip, regs);
            break;
        case Instruction::NERI:
            interpret_neri(ip, regs);
            break;
        // Noncommutative binary instructions.
        case Instruction::SUBRR:
            interpret_subrr(ip, regs);
            break;
        case Instruction::DIVRR:
            interpret_divrr(ip, regs);
            break;
        case Instruction::MODRR:
            interpret_modrr(ip, regs);
            break;
        case Instruction::LTRR:
            interpret_ltrr(ip, regs);
            break;
        case Instruction::LERR:
            interpret_lerr(ip, regs);
            break;
        case Instruction::SUBRI:
            interpret_subri(ip, regs);
            break;
        case Instruction::DIVRI:
            interpret_divri(ip, regs);
            break;
        case Instruction::MODRI:
            interpret_modri(ip, regs);
            break;
        case Instruction::LTRI:
            interpret_ltri(ip, regs);
            break;
        case Instruction::LERI:
            interpret_leri(ip, regs);
            break;
        case Instruction::SUBIR:
            interpret_subir(ip, regs);
            break;
        case Instruction::DIVIR:
            interpret_divir(ip, regs);
            break;
        case Instruction::MODIR:
            interpret_modir(ip, regs);
            break;
        case Instruction::LTIR:
            interpret_ltir(ip, regs);
            break;
        case Instruction::LEIR:
            interpret_leir(ip, regs);
            break;
        // Unary instructions.
        case Instruction::NEG:
            interpret_neg(ip, regs);
            break;
        case Instruction::NOT:
            interpret_not(ip, regs);
            break;
        // Move instructions.
        case Instruction::MOVI:
            interpret_movi(ip, regs);
            break;
        case Instruction::MOVR:
            interpret_movr(ip, regs);
            break;
        // Jump instructions.
        case Instruction::JMP:
            interpret_jmp(ip);
            break;
        case Instruction::JT:
            interpret_jt(ip, regs);
            break;
        case Instruction::JF:
            interpret_jf(ip, regs);
            break;
        // Call/ret instructions.
        case Instruction::CALL:
            interpret_call(ip, regs);
            break;
        case Instruction::RETR:
            interpret_retr(ip, regs);
            break;
        case Instruction::RETI:
            interpret_reti(ip, regs);
            break;
        // System instructions.
        case Instruction::EXIT:
            return interpret_exit(ip, regs);
        case Instruction::IN:
            interpret_in(ip, regs);
            break;
        case Instruction::OUT:
            interpret_out(ip, regs);
            break;
        default:
            UNREACHABLE();
        }
    }
    UNREACHABLE();
}

#endif // INTERPRETER_REPLICATE_SWITCH
