#ifndef EXPR_HPP
#define EXPR_HPP

class Val;
class Env;

#include "pointer.h"
#include <string>
#include <sstream>

enum precedence_t {
    precedence_none = 0,
    precedence_add = 2,
    precedence_mult = 3,
};

CLASS(Expr) {
public:
    virtual bool equals(PTR(Expr) e) = 0;

    virtual PTR(Val) interp(PTR(Env) env = nullptr) = 0;

    std::string to_string() {
        std::stringstream st("");
        this->print(st);
        return st.str();
    }

    virtual void print(std::ostream &out) = 0;

    std::string to_pretty_string() {
        std::stringstream st("");
        this->pretty_print(st);
        return st.str();
    }

    void pretty_print(std::ostream &out) {
        this->pretty_print_at(out, precedence_none, false, false, out.tellp());
    };

    virtual void
    pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq,
                    int prev_stop_at) = 0;

    virtual ~Expr() = default;;
};

class NumExpr : public Expr {
public:
    int val;

    explicit NumExpr(int val);

    bool equals(PTR(Expr) e);

    PTR(Val) interp(PTR(Env) env = nullptr);

    void print(std::ostream &out);

    void
    pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq, int prev_stop_at);
};

class AddExpr : public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;

    AddExpr(PTR(Expr) lhs, PTR(Expr) rhs);

    bool equals(PTR(Expr) e);

    PTR(Val) interp(PTR(Env) env = nullptr);

    void print(std::ostream &out);

    void
    pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq, int prev_stop_at);
};

class MultExpr : public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;

    MultExpr(PTR(Expr) lhs, PTR(Expr) rhs);

    bool equals(PTR(Expr) e);

    PTR(Val) interp(PTR(Env) env = nullptr);

    void print(std::ostream &out);

    void
    pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq, int prev_stop_at);
};

class VarExpr : public Expr {
public:
    std::string variable;

    VarExpr(std::string);

    bool equals(PTR(Expr) e);

    PTR(Val) interp(PTR(Env) env = nullptr);

    void print(std::ostream &out);

    void
    pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq, int prev_stop_at);
};

class LetExpr : public Expr {
public:
    std::string lhs;
    PTR(Expr) rhs;
    PTR(Expr) body;

    LetExpr(std::string lhs, PTR(Expr) rhs, PTR(Expr) body);

    bool equals(PTR(Expr) e);

    PTR(Val) interp(PTR(Env) env = nullptr);

    void print(std::ostream &out);

    void
    pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq, int prev_stop_at);
};

class BoolExpr : public Expr {
public:
    bool rep;

    BoolExpr(bool rep);

    bool equals(PTR(Expr) e);

    PTR(Val) interp(PTR(Env) env = nullptr);

    void print(std::ostream &out);

    void
    pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq, int prev_stop_at);
};

class IfExpr : public Expr {
public:
    PTR(Expr) condition;
    PTR(Expr) then_expr;
    PTR(Expr) else_expr;

    IfExpr(PTR(Expr) condition, PTR(Expr) then_expr, PTR(Expr) else_expr);

    bool equals(PTR(Expr) e);

    PTR(Val) interp(PTR(Env) env = nullptr);

    void print(std::ostream &out);

    void
    pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq, int prev_stop_at);
};

class EqExpr : public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;

    EqExpr(PTR(Expr) lhs, PTR(Expr) rhs);

    bool equals(PTR(Expr) e);

    PTR(Val) interp(PTR(Env) env = nullptr);

    void print(std::ostream &out);

    void
    pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq, int prev_stop_at);
};

class FunExpr : public Expr {
public:
    std::string formal_arg;
    PTR(Expr) body;

    FunExpr(std::string formal_arg, PTR(Expr) body);

    bool equals(PTR(Expr) e);

    PTR(Val) interp(PTR(Env) env = nullptr);

    void print(std::ostream &out);

    void
    pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq, int prev_stop_at);
};

class CallExpr : public Expr {
public:
    PTR(Expr) to_be_called;
    PTR(Expr) actual_arg;

    CallExpr(PTR(Expr) to_be_called, PTR(Expr) actual_arg);

    bool equals(PTR(Expr) e);

    PTR(Val) interp(PTR(Env) env = nullptr);

    void print(std::ostream &out);

    void
    pretty_print_at(std::ostream &out, precedence_t precedence, bool wrap_let_or_fun, bool wrap_eq, int prev_stop_at);
};


#endif // EXPR_HPP
