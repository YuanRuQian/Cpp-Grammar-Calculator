#ifndef VAL_HPP
#define VAL_HPP


class Expr;
class Env;

#include "pointer.h"
#include <stdexcept>
#include <string>

CLASS(Val) {
public:
    virtual PTR(Val) add_to(PTR(Val) other_val) = 0;

    virtual PTR(Val) mult_with(PTR(Val) other_val) = 0;

    virtual bool equals(PTR(Val) other_val) = 0;

    virtual std::string to_string() = 0;

    virtual bool is_true() = 0;

    virtual PTR(Val) call(PTR(Val) actual_arg) = 0;

    virtual ~Val() = default;
};

class NumVal : public Val {
public:
    int rep;

    explicit NumVal(int rep);

    PTR(Val) add_to(PTR(Val) other_val);

    PTR(Val) mult_with(PTR(Val) other_val);

    bool equals(PTR(Val) other_val);

    std::string to_string();

    bool is_true();

    PTR(Val) call(PTR(Val) actual_arg);
};


class BoolVal : public Val {
public:
    bool rep;

    explicit BoolVal(bool rep);

    PTR(Val) add_to(PTR(Val) other_val);

    PTR(Val) mult_with(PTR(Val) other_val);

    bool equals(PTR(Val) other_val);

    std::string to_string();

    bool is_true();

    PTR(Val) call(PTR(Val) actual_arg);
};

class FunVal : public Val {
public:
    std::string formal_arg;
    PTR(Expr) body;
    PTR(Env) env;

    explicit FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env = nullptr);

    PTR(Val) add_to(PTR(Val) other_val);

    PTR(Val) mult_with(PTR(Val) other_val);

    bool equals(PTR(Val) other_val);

    std::string to_string();

    bool is_true();

    PTR(Val) call(PTR(Val) actual_arg);
};

#endif // VAL_HPP
