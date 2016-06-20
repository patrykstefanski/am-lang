#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <vector>
#include "instruction.hpp"

// Interprets the bytecode, returns the exit code of the interpreted program.
int interpret(const std::vector<Instruction>& bytecode);

#endif // !INTERPRETER_HPP
