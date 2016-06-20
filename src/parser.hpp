#ifndef PARSER_HPP
#define PARSER_HPP

#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>
#include "instruction.hpp"
#include "lexer.hpp"

class Parser final {
public:
    explicit Parser(Lexer lexer);
    Parser(const Parser&) = delete;
    void operator=(const Parser&) = delete;

    // Parses the source.
    void parse();

    const std::vector<Instruction>& bytecode() const;

private:
    enum Operator {
        // Commutative binary operators.
        ADD,
        MUL,
        EQ,
        NE,
        // Noncommutative binary operators.
        SUB,
        DIV,
        MOD,
        LT,
        LE,
        // Other operators without corresponding instructions.
        GT,
        GE,
        // Unary operators.
        NEG,
        NOT,
    };

    // These constraints must be satisfied, otherwise some methods will
    // emit wrong code.
    static_assert(MUL - ADD == Instruction::MULRR - Instruction::ADDRR);
    static_assert(EQ - ADD == Instruction::EQRR - Instruction::ADDRR);
    static_assert(NE - ADD == Instruction::NERR - Instruction::ADDRR);
    static_assert(MUL - ADD == Instruction::MULRI - Instruction::ADDRI);
    static_assert(EQ - ADD == Instruction::EQRI - Instruction::ADDRI);
    static_assert(NE - ADD == Instruction::NERI - Instruction::ADDRI);
    static_assert(DIV - SUB == Instruction::DIVRR - Instruction::SUBRR);
    static_assert(MOD - SUB == Instruction::MODRR - Instruction::SUBRR);
    static_assert(LT - SUB == Instruction::LTRR - Instruction::SUBRR);
    static_assert(LE - SUB == Instruction::LERR - Instruction::SUBRR);
    static_assert(DIV - SUB == Instruction::DIVRI - Instruction::SUBRI);
    static_assert(MOD - SUB == Instruction::MODRI - Instruction::SUBRI);
    static_assert(LT - SUB == Instruction::LTRI - Instruction::SUBRI);
    static_assert(LE - SUB == Instruction::LERI - Instruction::SUBRI);
    static_assert(DIV - SUB == Instruction::DIVIR - Instruction::SUBIR);
    static_assert(MOD - SUB == Instruction::MODIR - Instruction::SUBIR);
    static_assert(LT - SUB == Instruction::LTIR - Instruction::SUBIR);
    static_assert(LE - SUB == Instruction::LEIR - Instruction::SUBIR);
    static_assert(GE - GT == LE - LT);
    static_assert(NOT - NEG == Instruction::NOT - Instruction::NEG);

    struct Expression final {
        constexpr Expression() : value_(0), has_reg_(false) {}
        constexpr Expression(const Expression&) = default;
        constexpr Expression& operator=(const Expression&) = default;

        // Makes a value expression.
        constexpr static Expression make_value(std::int64_t value) {
            Expression expr;
            expr.value_ = value;
            expr.has_reg_ = false;
            return expr;
        }

        // Makes an expression with a value.
        constexpr static Expression make_reg(std::uint8_t reg) {
            Expression expr;
            expr.reg_ = reg;
            expr.has_reg_ = true;
            return expr;
        }

        constexpr std::int64_t value() const {
            return value_;
        }

        constexpr std::uint8_t reg() const {
            return reg_;
        }

        constexpr bool has_reg() const {
            return has_reg_;
        }

    private:
        union {
            std::int64_t value_;
            std::uint8_t reg_;
        };
        bool has_reg_;
    };

    struct Scope final {
        constexpr Scope() : first_free_reg_(0), num_variables_(0) {}
        constexpr Scope(const Scope&) = default;
        constexpr Scope& operator=(const Scope&) = default;

        std::size_t first_free_reg_;
        std::size_t num_variables_;
    };

    // Finds the register where the given variable (symbol_id) is stored.
    // Returns the variable register if the variable exists in the current
    // scope, std::numeric_limits<std::size_t>::max() otherwise.
    std::size_t find_variable_reg(std::size_t symbol_id) const;

    // Stores the expression into the given register.
    Expression expr_to_reg(Expression expr, std::uint8_t reg);

    // Stores the expression into the next free register.
    Expression expr_to_next_reg(Expression expr);

    // Stores the expression into any register.
    Expression expr_to_any_reg(Expression expr);

    // Frees the expression register if any.
    void free_expr_reg(Expression expr);

    // Gets the next jump.
    std::size_t next_jump(std::size_t pos) const;

    // Appends the position in the bytecode to the jump list.
    void append_jump(std::size_t* list, std::size_t pos);

    // Patches the single instruction at given position to the target.
    void patch_single_jump(std::size_t pos, std::size_t target);

    // Patches the jump list to the given target.
    void patch_jump_list(std::size_t list, std::size_t target);

    // Patches the jump list to the current program counter.
    void patch_jump_list_to_here(std::size_t list);

    // Folds a binary operator.
    static Expression fold_unary_op(int op, Expression expr);

    // Folds a binary operator.
    static Expression fold_binary_op(int op, Expression lhs, Expression rhs);

    // Emits an unary operator.
    Expression emit_unary_op(int op, Expression expr);

    // Emits a commutative binary operator.
    Expression emit_commutative_op(int op, Expression lhs, Expression rhs);

    // Emits a noncommutative binary operator.
    Expression emit_noncommutative_op(int op, Expression lhs, Expression rhs);

    // Emits a binary operator. None will be emitted if the both expressions can
    // be folded.
    Expression emit_binary_op(int op, Expression lhs, Expression rhs);

    // Emits an unconditional jump.
    std::size_t emit_unconditional_jump();

    // Emits a conditional jump. The 'reg' register will be compared.
    // If confition is true, then JT will be generated, JF otherwise.
    std::size_t emit_conditional_jump(std::uint8_t reg, bool condition);

    // Emits a call instruction.
    Expression emit_call(std::size_t symbol_id, std::uint8_t reg,
            std::size_t num_args);

    // Emits a ret instruction.
    void emit_return(Expression expr);
    
    // Emits an in/out instruction.
    void emit_io(Instruction::Opcode, std::uint8_t reg);

    // Parses function parameters, returns the number of them.
    // parameters -> <none> | expr { ',' expr }
    std::size_t parse_parameters();

    // Parses an identifier expression. It can be a variable or a function call.
    // identifier_expr -> IDENTIFIER [ '(' parameters ')' ]
    Expression parse_identifier_expr();

    // Parses an integer literal expression.
    // integer_literal_expr -> INTEGER_LITERAL
    Expression parse_integer_literal_expr();

    // Parses a parentheses expression.
    // parentheses_expr -> '(' expr ')'
    Expression parse_parentheses_expr();

    // Parses a primary expression.
    // primary_expr -> identifier_expr | integer_literal_expr | parentheses_expr
    Expression parse_primary_expr();

    // Parses an unary expression.
    // unary_expr -> [ '!' | '-' ] primary_expr
    Expression parse_unary_expr();

    // Converts token to operator.
    static int token_to_binary_op(int token);

    // Parses a binary expression with higher precedence than limit.
    // binary_expr -> unary_expr [ ( '+', '-', ...) binary_expr ]
    Expression parse_binary_expr(std::size_t limit);

    // Parses an expression.
    // expr -> binary_expr
    Expression parse_expr();

    // Parses a block statement.
    // block -> '{' { statement } '}'
    void parse_block();

    // Parses a conditional block for IF / ELSE IF, returns the jump list
    // for false conditions.
    // cond_block -> expr block
    std::size_t parse_cond_block();

    // Parses an if statement.
    // if -> IF cond_block { ELSE IF cond_block } [ ELSE block ]
    void parse_if();

    // Parses an in statement.
    // in -> IN IDENTIFIER ';'
    void parse_in();

    // Parses a let statement.
    // let -> LET IDENTIFIER '=' expr ';'
    void parse_let();

    // Parses an out statement.
    // out -> OUT expr ';'
    void parse_out();

    // Parses a return statement.
    // return -> RETURN expr ';'
    void parse_return();

    // Parses a while statement.
    // while -> WHILE cond_block
    void parse_while();

    // Parses an assignment or a call.
    // assignment_or_call -> IDENTIFIER '=' expr ';' |
    //                       IDENTIFIER '(' parameters ')' ';'
    void parse_assignment_or_call();

    // Parses a statement.
    // statement -> block | if | in | let | out | return | while |
    //              assignment_or_call
    int parse_statement();

    // Parses function arguments, returns the number of them.
    // arguments -> <none> | IDENTIFIER { ',' IDENTIFIER }
    std::size_t parse_arguments();
    void parse_fn();
    void first_pass();
    void second_pass();

    std::size_t variable_regs_[0xff];
    // Map of defined functions with their positions in the bytecode and number
    // of arguments.
    std::unordered_map<std::size_t, std::pair<std::size_t, std::size_t>>
        functions_;
    Lexer lexer_;
    std::vector<Instruction> bytecode_;
    Scope* current_scope_;
};

#endif // !PARSER_HPP
