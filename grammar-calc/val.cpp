#include "expr.hpp"
#include "val.hpp"
#include "env.h"

#include <utility>

NumVal::NumVal(int rep) {
    this->rep = rep;
}

PTR(Val)  NumVal::add_to(PTR(Val) other_val) {
    auto other_num = CAST(NumVal)(other_val);
    if (other_num == nullptr) {
        throw std::runtime_error("add to non-number");
    }
    int new_val = (unsigned) this->rep + (unsigned) other_num->rep;
    return NEW(NumVal)(new_val);
}

PTR(Val)  NumVal::mult_with(PTR(Val) other_val) {
    auto other_num = CAST(NumVal)(other_val);
    if (other_num == nullptr) {
        throw std::runtime_error("mult with non-number");
    }
    int new_val = (unsigned) this->rep * (unsigned) other_num->rep;
    return NEW(NumVal)(new_val);
}

bool NumVal::equals(PTR(Val) other_val) {
    auto other_num = CAST(NumVal)(other_val);
    if (other_num == nullptr) {
        return false;
    }
    return this->rep == other_num->rep;
}

std::string NumVal::to_string() {
    return std::to_string(this->rep);
}

bool NumVal::is_true() {
    throw std::runtime_error("a num val cannot be interpreted as a bool val");
}

PTR(Val)  NumVal::call(PTR(Val) actual_arg) {
    throw std::runtime_error("cannot call on a num val");
}

BoolVal::BoolVal(bool rep) {
    this->rep = rep;
}

PTR(Val)  BoolVal::add_to(PTR(Val) other_val) {
    throw std::runtime_error("cannot add to a bool val");
}

PTR(Val)  BoolVal::mult_with(PTR(Val) other_val) {
    throw std::runtime_error("cannot mult with a bool val");
}

bool BoolVal::equals(PTR(Val) other_val) {
    auto other_bool = CAST(BoolVal)(other_val);
    if (other_bool == nullptr) {
        return false;
    }
    return this->rep == other_bool->rep;
}

std::string BoolVal::to_string() {
    return this->rep ? "_true" : "_false";
}

bool BoolVal::is_true() {
    return this->rep;
}

PTR(Val)  BoolVal::call(PTR(Val) actual_arg) {
    throw std::runtime_error("cannot call on a bool val");
}

FunVal::FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env) {
    if(env == nullptr) {
        env = Env::empty;
    }
    this->formal_arg = std::move(formal_arg);
    this->body = std::move(body);
    this->env = std::move(env);
}

PTR(Val)  FunVal::add_to(PTR(Val) other_val) {
    throw std::runtime_error("cannot add to a fun val");
}

PTR(Val)  FunVal::mult_with(PTR(Val) other_val) {
    throw std::runtime_error("cannot mult with a fun val");
}

bool FunVal::equals(PTR(Val) other_val) {
    auto other_fun = CAST(FunVal)(other_val);
    if (other_fun == nullptr) {
        return false;
    }
    return this->formal_arg == other_fun->formal_arg && this->body->equals(other_fun->body);
}

std::string FunVal::to_string() {
    return "[function]";
}

bool FunVal::is_true() {
    throw std::runtime_error("a fun val cannot be interpreted as a bool val");
}

PTR(Val) FunVal::call(PTR(Val) actual_arg) {
    return this->body->interp(NEW(ExtendedEnv)(this->formal_arg, actual_arg, this->env));
}
