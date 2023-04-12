#include "parse.h"
#include "expr.hpp"

PTR(Expr) parse_expression_str(const std::string &str) {
    std::istringstream is(str);
    return parse_expr(is);
}

// expr: comparg || comparg == expr
PTR(Expr) parse_expr(std::istream &in, int open_parenthesis_to_match) {
    // std::cout << "parse_expr:\n";
    PTR(Expr) comprag = parse_comprag(in, open_parenthesis_to_match);
    skip_whitespaces(in, open_parenthesis_to_match);
    int ch = in.peek();
    if (ch == '=') {
        consume_word(in, "==", open_parenthesis_to_match);
        PTR(Expr) second_expr = parse_expr(in, open_parenthesis_to_match);
        comprag = NEW(EqExpr)(comprag, second_expr);
        skip_whitespaces(in, open_parenthesis_to_match);
        ch = in.peek();
    }

    if (ch != EOF && ch != ')' && ch != '_' && ch != '\n' && ch != '(') {
        throw std::runtime_error("invalid input");
    }
    if (open_parenthesis_to_match == 0 && ch == ')') {
        throw std::runtime_error("missing open parenthesis");
    }
    return comprag;
}

// comprag: addend | addend + comprag
PTR(Expr) parse_comprag(std::istream &in, int &open_parenthesis_to_match) {
    //   std::cout << "parse_comprag:\n";
    PTR(Expr) addend = parse_addend(in, open_parenthesis_to_match);
    skip_whitespaces(in, open_parenthesis_to_match);
    int ch = in.peek();
    if (ch == '+') {
        consume(in, '+', open_parenthesis_to_match);
        PTR(Expr) second_expr = parse_comprag(in, open_parenthesis_to_match);
        addend = NEW(AddExpr)(addend, second_expr);
        skip_whitespaces(in, open_parenthesis_to_match);
    }
    return addend;
}

PTR(Expr) parse_num(std::istream &in, int &open_parenthesis_to_match) {
    unsigned int num = 0;
    bool is_negative = false;
    if (in.peek() == '-') {
        consume(in, '-', open_parenthesis_to_match);
        is_negative = true;
        if (!isdigit(in.peek())) {
            throw std::runtime_error("number should come right after -");
        }
    }
    int ch;

    while ((ch = in.peek()) && isdigit(ch)) {
        consume(in, ch, open_parenthesis_to_match);
        num = num * 10 + (ch - '0');
    }

    if (is_negative) {
        num *= -1;
    }
    return NEW(NumExpr)((int) num);
}


// addend: multiplicand | multiplicand * addend
PTR(Expr) parse_addend(std::istream &in, int &open_parenthesis_to_match) {
    // std::cout << "parse_addend\n";
    PTR(Expr) first_multiplicand = parse_multiplicand(in, open_parenthesis_to_match);
    skip_whitespaces(in, open_parenthesis_to_match);

    int ch = in.peek();

    if (ch != '*') {
        return first_multiplicand;
    }
    consume(in, '*', open_parenthesis_to_match);
    skip_whitespaces(in, open_parenthesis_to_match);
    PTR(Expr) second_addend = parse_addend(in, open_parenthesis_to_match);
    return NEW(MultExpr)(first_multiplicand, second_addend);
}

PTR(Expr) parse_variable(std::istream &in, int &open_parenthesis_to_match) {
    int ch;
    std::string str;

    while ((ch = in.peek()) && isalpha(ch)) {
        consume(in, ch, open_parenthesis_to_match);
        str += (char) ch;
    }
    ch = in.peek();

    if (!isspace(ch) && !(ch == '+' || ch == '*' || ch == ')' || ch == '(' || ch == '=' || in.eof())) {
        throw std::runtime_error("unexpected character in variable");
    }
    return NEW(VarExpr)(str);
}

void consume_word(std::istream &in, const std::string &expectation, int &open_parenthesis_to_match) {
    for (char ch: expectation) {
        if (in.peek() != ch) {
            throw std::runtime_error("invalid input");
        }
        consume(in, ch, open_parenthesis_to_match);
    }
}


PTR(Expr) parse_let_binding(std::istream &in, int &open_parenthesis_to_match) {
    skip_whitespaces(in, open_parenthesis_to_match);
    PTR(Expr) lhs = parse_variable(in, open_parenthesis_to_match);
    skip_whitespaces(in, open_parenthesis_to_match);
    consume_word(in, "=", open_parenthesis_to_match);
    skip_whitespaces(in, open_parenthesis_to_match);
    PTR(Expr) rhs = parse_comprag(in, open_parenthesis_to_match);
    skip_whitespaces(in, open_parenthesis_to_match);
    // std::cout << "parse_comprag: " << rhs->to_string() << "\n";
    consume_word(in, "_in", open_parenthesis_to_match);
    skip_whitespaces(in, open_parenthesis_to_match);
    PTR(Expr) body = parse_comprag(in, open_parenthesis_to_match);
    skip_whitespaces(in, open_parenthesis_to_match);
    return NEW(LetExpr)(lhs->to_string(), rhs, body);
}

PTR(Expr) parse_if_expr(std::istream &in, int &open_parenthesis_to_match) {
    // std::cout << "parse_if_expr:\n";
    skip_whitespaces(in, open_parenthesis_to_match);
    //  std::cout << "parse_if_expr: after skip_whitespaces\n";
    PTR(Expr) condition = parse_expr(in, open_parenthesis_to_match);
    // std::cout << "parse_if_expr condition: " << condition->to_string() << "\n";
    skip_whitespaces(in, open_parenthesis_to_match);
    consume_word(in, "_then", open_parenthesis_to_match);
    skip_whitespaces(in, open_parenthesis_to_match);
    PTR(Expr) then_expr = parse_expr(in, open_parenthesis_to_match);
    skip_whitespaces(in, open_parenthesis_to_match);
    consume_word(in, "_else", open_parenthesis_to_match);
    skip_whitespaces(in, open_parenthesis_to_match);
    PTR(Expr) else_expr = parse_expr(in, open_parenthesis_to_match);
    skip_whitespaces(in, open_parenthesis_to_match);
    return NEW(IfExpr)(condition, then_expr, else_expr);
}

// multiplicand:  〈inner〉 | 〈multicand〉 ( 〈expr〉 )
PTR(Expr) parse_multiplicand(std::istream &in, int &open_parenthesis_to_match) {
    // std::cout << "parse_multiplicand:\n";
    skip_whitespaces(in, open_parenthesis_to_match);
    PTR(Expr) expr = parse_inner(in, open_parenthesis_to_match);
    // std::cout << "inner: " << expr->to_string() << "\n";
    skip_whitespaces(in, open_parenthesis_to_match);
    while (!in.eof() && in.peek() == '(') {
        consume(in, '(', open_parenthesis_to_match);
        skip_whitespaces(in, open_parenthesis_to_match);
        PTR(Expr) actual_arg = parse_expr(in, open_parenthesis_to_match);
        skip_whitespaces(in, open_parenthesis_to_match);
        consume(in, ')', open_parenthesis_to_match);
        expr = NEW(CallExpr)(expr, actual_arg);
    }
    return expr;
}

// inner: number | ( expression ) | variable | let binding | _true | _false | _if _then _else | _fun ( 〈variable〉 ) 〈expr〉
PTR(Expr) parse_inner(std::istream &in, int &open_parenthesis_to_match) {
    //  std::cout << "parse_inner:\n";
    skip_whitespaces(in, open_parenthesis_to_match);
    int ch = in.peek();
    // std::cout << "parse_inner ch: " << (char) ch << "   " << (int) ch << "\n";
    if (isdigit(ch) || ch == '-') {
        return parse_num(in, open_parenthesis_to_match);
    }
    if (ch == '(') {
        consume(in, '(', open_parenthesis_to_match);
        PTR(Expr) inner_expr = parse_expr(in, open_parenthesis_to_match);
        skip_whitespaces(in, open_parenthesis_to_match);
        if (open_parenthesis_to_match > 0 && in.peek() != ')') {
            if (in.eof()) {
                throw std::runtime_error("missing close parenthesis");
            }
        } else {
            consume(in, ')', open_parenthesis_to_match);
        }
        return inner_expr;
    }

    if (isalpha(ch)) {
        PTR(Expr) variable = parse_variable(in, open_parenthesis_to_match);
        skip_whitespaces(in, open_parenthesis_to_match);
        return variable;
    }

    if (ch == '_') {
        std::string next_keyword = consume_and_find_next_keyword(in, open_parenthesis_to_match);
        if (next_keyword == "_let") {
            return parse_let_binding(in, open_parenthesis_to_match);
        } else if (next_keyword == "_false") {
            return NEW(BoolExpr)(false);
        } else if (next_keyword == "_true") {
            return NEW(BoolExpr)(true);
        } else if (next_keyword == "_if") {
            return parse_if_expr(in, open_parenthesis_to_match);
        } else if (next_keyword == "_fun") {
            return parse_fun_expr(in, open_parenthesis_to_match);
        }
    }
    consume(in, ch, open_parenthesis_to_match);
    throw std::runtime_error("invalid input");
}

PTR(Expr) parse_fun_expr(std::istream &in, int &open_parenthesis_to_match) {
    skip_whitespaces(in, open_parenthesis_to_match);
    consume(in, '(', open_parenthesis_to_match);
    skip_whitespaces(in, open_parenthesis_to_match);
    PTR(Expr) variable = parse_variable(in, open_parenthesis_to_match);
    skip_whitespaces(in, open_parenthesis_to_match);
    consume(in, ')', open_parenthesis_to_match);
    PTR(Expr) body = parse_expr(in, open_parenthesis_to_match);
    return NEW(FunExpr)(variable->to_string(), body);
}

void consume(std::istream &in, int expectation, int &open_parenthesis_to_match) {
    int ch = in.get();
    if (ch != expectation) {
        throw std::runtime_error("consume mismatch");
    }
    if (ch == '(') {
        open_parenthesis_to_match++;
    }
    if (ch == ')') {
        open_parenthesis_to_match--;
    }
    // std::cout << "consume: real: " << (char) ch << " expected: " << (char) expectation << "\n";
}

void skip_whitespaces(std::istream &in, int &open_parenthesis_to_match) {
    int ch;
    while ((ch = in.peek()) && isspace(ch)) {
        consume(in, ch, open_parenthesis_to_match);
    }
}

// start with _, consume and return the keyword like _let: return _let
std::string consume_and_find_next_keyword(std::istream &in, int &open_parenthesis_to_match) {
    if (in.eof() || in.peek() != '_') {
        throw std::runtime_error("not a keyword");
    }
    consume(in, '_', open_parenthesis_to_match);
    std::string keyword = "_";
    while (isalpha(in.peek())) {
        keyword += (char) in.peek();
        consume(in, in.peek(), open_parenthesis_to_match);
    }
    return keyword;
}
