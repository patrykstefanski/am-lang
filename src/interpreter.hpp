#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <cstdint>
#include <vector>
#include "instruction.hpp"

// Interprets the bytecode, returns the exit code of the interpreted program.
int interpret(const std::vector<Instruction>& bytecode,
        const std::vector<std::int64_t>& constants);

#endif // !INTERPRETER_HPP
