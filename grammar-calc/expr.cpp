#include "expr.hpp"
#include "val.hpp"
#include "env.h"
#include <utility>

NumExpr::NumExpr(int val) {
    this->val = val;
}

bool NumExpr::equals(PTR(Expr) e) {
    auto other = CAST(NumExpr)(e);
    if (other == nullptr) {
        return false;
    }
    return this->val == other->val;
}

PTR(Val)NumExpr::interp(PTR(Env) env) {
    return NEW(NumVal)(this->val);
}

void NumExpr::print(std::ostream &out) {
    out << std::to_string(val);
}

void
NumExpr::pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq,
                         int prev_stop_at) {
    out << std::to_string(val);
}

AddExpr::AddExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    this->lhs = std::move(lhs);
    this->rhs = std::move(rhs);
}

bool AddExpr::equals(PTR(Expr) e) {
    auto other = CAST(AddExpr)(e);
    if (other == nullptr) {
        return false;
    }
    return this->lhs->equals(other->lhs) && this->rhs->equals(other->rhs);
}

PTR(Val)AddExpr::interp(PTR(Env) env) {
    if(env == nullptr) {
        env = Env::empty;
    }
    return this->lhs->interp(env)->add_to(this->rhs->interp(env));
}

void AddExpr::print(std::ostream &out) {
    out << "(";
    this->lhs->print(out);
    out << "+";
    this->rhs->print(out);
    out << ")";
}

void AddExpr::pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq,
                              int prev_stop_at) {
    if (precedence >= precedence_add) {
        out << "(";
    }
    this->lhs->pretty_print_at(out, precedence_add, true, true, prev_stop_at);
    out << " + ";
    this->rhs->pretty_print_at(out, precedence_none, false, true, prev_stop_at);
    if (precedence >= precedence_add) {
        out << ")";
    }
}

MultExpr::MultExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    this->lhs = std::move(lhs);
    this->rhs = std::move(rhs);
}

bool MultExpr::equals(PTR(Expr) e) {
    auto other = CAST(MultExpr)(e);
    if (other == nullptr) {
        return false;
    }
    return this->lhs->equals(other->lhs) && this->rhs->equals(other->rhs);
}

PTR(Val)MultExpr::interp(PTR(Env) env) {
    if(env == nullptr) {
        env = Env::empty;
    }
    return this->lhs->interp(env)->mult_with(this->rhs->interp(env));
}

void MultExpr::print(std::ostream &out) {
    out << "(";
    this->lhs->print(out);
    out << "*";
    this->rhs->print(out);
    out << ")";
}

void MultExpr::pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq,
                               int prev_stop_at) {
    bool mult_add_parentheses = precedence >= precedence_mult;
    if (mult_add_parentheses) {
        out << "(";
    }
    this->lhs->pretty_print_at(out, precedence_mult, true, true, prev_stop_at);
    out << " * ";
    this->rhs->pretty_print_at(out, precedence_add, !mult_add_parentheses && wrap_let_or_fun, true, prev_stop_at);
    if (mult_add_parentheses) {
        out << ")";
    }
}

VarExpr::VarExpr(std::string variable) {
    this->variable = std::move(variable);
}

bool VarExpr::equals(PTR(Expr) e) {
    auto other = CAST(VarExpr)(e);
    if (other == nullptr) {
        return false;
    }
    return this->variable == other->variable;
}

PTR(Val)VarExpr::interp(PTR(Env) env) {
    if(env == nullptr) {
        env = Env::empty;
    }
    return env->lookup(this->variable);
}

void VarExpr::print(std::ostream &out) {
    out << this->variable;
}

void
VarExpr::pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq,
                         int prev_stop_at) {
    out << this->variable;
}

LetExpr::LetExpr(std::string lhs, PTR(Expr) rhs, PTR(Expr) body) {
    this->lhs = std::move(lhs);
    this->rhs = std::move(rhs);
    this->body = std::move(body);
}

bool LetExpr::equals(PTR(Expr) e) {
    auto other = CAST(LetExpr)(e);
    if (other == nullptr) {
        return false;
    }
    return this->lhs == other->lhs && this->rhs->equals(other->rhs) && this->body->equals(other->body);
}

PTR(Val)LetExpr::interp(PTR(Env) env) {
    if(env == nullptr) {
        env = Env::empty;
    }
    PTR(Val) rhs_val = this->rhs->interp(env);
    PTR(Env) new_env = NEW(ExtendedEnv)(lhs, rhs_val, env);
    return body->interp(new_env);
}

void LetExpr::print(std::ostream &out) {
    out << "(_let " << this->lhs << "=";
    this->rhs->print(out);
    out << " _in ";
    this->body->print(out);
    out << ")";
}

void
LetExpr::pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq,
                         int prev_stop_at) {
    if (wrap_let_or_fun) {
        out << "(";
    }
    int blank_spaces_backoff = (int) out.tellp() - prev_stop_at;
    out << "_let " << this->lhs << " = ";
    this->rhs->pretty_print_at(out, precedence_none, false, false, prev_stop_at);
    out << "\n";
    prev_stop_at = out.tellp();
    out << std::string(blank_spaces_backoff, ' ') << "_in  ";
    this->body->pretty_print_at(out, precedence_none, false, false, prev_stop_at);
    if (wrap_let_or_fun) {
        out << ")";
    }
}

BoolExpr::BoolExpr(bool rep) {
    this->rep = rep;
}

bool BoolExpr::equals(PTR(Expr) e) {
    auto other = CAST(BoolExpr)(e);
    if (other == nullptr) {
        return false;
    }
    return this->rep == other->rep;
}

PTR(Val)BoolExpr::interp(PTR(Env) env) {
    return NEW(BoolVal)(this->rep);
}

void BoolExpr::print(std::ostream &out) {
    if (this->rep) {
        out << "_true";
    } else {
        out << "_false";
    }
}

void
BoolExpr::pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq,
                          int prev_stop_at) {
    if (this->rep) {
        out << "_true";
    } else {
        out << "_false";
    }
}

IfExpr::IfExpr(PTR(Expr) condition, PTR(Expr) then_expr, PTR(Expr) else_expr) {
    this->condition = std::move(condition);
    this->then_expr = std::move(then_expr);
    this->else_expr = std::move(else_expr);
}

bool IfExpr::equals(PTR(Expr) e) {
    auto other = CAST(IfExpr)(e);
    if (other == nullptr) {
        return false;
    }
    return this->condition->equals(other->condition) && this->then_expr->equals(other->then_expr) &&
           this->else_expr->equals(other->else_expr);
}

PTR(Val)IfExpr::interp(PTR(Env) env) {
    if(env == nullptr) {
        env = Env::empty;
    }
    PTR(Val) condition_val = this->condition->interp(env);
    if (condition_val->is_true()) {
        return this->then_expr->interp(env);
    } else {
        return this->else_expr->interp(env);
    }
}

void IfExpr::print(std::ostream &out) {
    out << "(_if ";
    this->condition->print(out);
    out << " _then ";
    this->then_expr->print(out);
    out << " _else ";
    this->else_expr->print(out);
    out << ")";
}

void
IfExpr::pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq,
                        int prev_stop_at) {
    int blank_spaces_backoff = (int) out.tellp() - prev_stop_at;
    out << "_if ";
    this->condition->pretty_print_at(out, precedence_none, false, false, prev_stop_at);
    out << "\n";
    prev_stop_at = out.tellp();
    out << std::string(blank_spaces_backoff, ' ') << "_then ";
    this->then_expr->pretty_print_at(out, precedence_none, false, false, prev_stop_at);
    out << "\n";
    prev_stop_at = out.tellp();
    out << std::string(blank_spaces_backoff, ' ') << "_else ";
    this->else_expr->pretty_print_at(out, precedence_none, false, false, prev_stop_at);
}

EqExpr::EqExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    this->lhs = std::move(lhs);
    this->rhs = std::move(rhs);
}

bool EqExpr::equals(PTR(Expr) e) {
    auto other = CAST(EqExpr)(e);
    if (other == nullptr) {
        return false;
    }
    return this->lhs->equals(other->lhs) && this->rhs->equals(other->rhs);
}

PTR(Val)EqExpr::interp(PTR(Env) env) {
    if(env == nullptr) {
        env = Env::empty;
    }
    PTR(Val) lhs_val = this->lhs->interp(env);
    PTR(Val) rhs_val = this->rhs->interp(env);
    bool result = lhs_val->equals(rhs_val);
    return NEW(BoolVal)(result);
}

void EqExpr::print(std::ostream &out) {
    out << "(";
    this->lhs->print(out);
    out << "==";
    this->rhs->print(out);
    out << ")";
}

void
EqExpr::pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq,
                        int prev_stop_at) {
    if (wrap_eq) {
        out << "(";
    }
    this->lhs->pretty_print_at(out, precedence_none, true, true, prev_stop_at);
    out << " == ";
    this->rhs->pretty_print_at(out, precedence_none, false, false, prev_stop_at);
    if (wrap_eq) {
        out << ")";
    }
}

FunExpr::FunExpr(std::string formal_arg, PTR(Expr) body) {
    this->formal_arg = std::move(formal_arg);
    this->body = std::move(body);
}

bool FunExpr::equals(PTR(Expr) e) {
    auto other = CAST(FunExpr)(e);
    if (other == nullptr) {
        return false;
    }
    return this->formal_arg == other->formal_arg && this->body->equals(other->body);
}

PTR(Val) FunExpr::interp(PTR(Env) env) {
    if(env == nullptr) {
        env = Env::empty;
    }
    return NEW(FunVal)(this->formal_arg, this->body, env);
}

void FunExpr::print(std::ostream &out) {
    out << "(_fun(" << this->formal_arg << ")";
    this->body->print(out);
    out << ")";
}

void
FunExpr::pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq,
                         int prev_stop_at) {
    if (wrap_let_or_fun) {
        out << "(";
    }
    int blank_spaces_backoff = (int) out.tellp() - prev_stop_at + 2;
    out << "_fun (" << this->formal_arg << ")\n";
    prev_stop_at = out.tellp();
    out << std::string(blank_spaces_backoff, ' ');
    this->body->pretty_print_at(out, precedence_none, false, false, prev_stop_at);
    if (wrap_let_or_fun) {
        out << ")";
    }
}

CallExpr::CallExpr(PTR(Expr) to_be_called, PTR(Expr) actual_arg) {
    this->to_be_called = std::move(to_be_called);
    this->actual_arg = std::move(actual_arg);
}

bool CallExpr::equals(PTR(Expr) e) {
    auto other = CAST(CallExpr)(e);
    if (other == nullptr) {
        return false;
    }
    return this->to_be_called->equals(other->to_be_called) && this->actual_arg->equals(other->actual_arg);
}

PTR(Val)CallExpr::interp(PTR(Env) env) {
    if(env == nullptr) {
        env = Env::empty;
    }
    return this->to_be_called->interp(env)
        ->call(this->actual_arg->interp(env));
}

void CallExpr::print(std::ostream &out) {
    out << "(";
    this->to_be_called->print(out);
    out << ") (";
    this->actual_arg->print(out);
    out << ")";
}


void CallExpr::pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq,
                               int prev_stop_at) {
    this->to_be_called->pretty_print_at(out, precedence_none, true, false, prev_stop_at);
    out << "(";
    this->actual_arg->pretty_print_at(out, precedence_none, false, false, prev_stop_at);
    out << ")";
}
