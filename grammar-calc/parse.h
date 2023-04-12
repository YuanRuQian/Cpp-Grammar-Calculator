#ifndef PARSE_H
#define PARSE_H

class Expr;

#include <iostream>
#include "pointer.h"

PTR(Expr) parse_expression_str(const std::string &str);

PTR(Expr) parse_expr(std::istream &in, int open_parenthesis_to_match = 0);

PTR(Expr) parse_comprag(std::istream &in, int &open_parenthesis_to_match);

PTR(Expr) parse_num(std::istream &in, int &open_parenthesis_to_match);

PTR(Expr) parse_addend(std::istream &in, int &open_parenthesis_to_match);

PTR(Expr) parse_multiplicand(std::istream &in, int &open_parenthesis_to_match);

PTR(Expr) parse_variable(std::istream &in, int &open_parenthesis_to_match);

PTR(Expr) parse_let_binding(std::istream &in, int &open_parenthesis_to_match);

PTR(Expr) parse_if_expr(std::istream &in, int &open_parenthesis_to_match);

PTR(Expr) parse_inner(std::istream &in, int &open_parenthesis_to_match);

PTR(Expr) parse_fun_expr(std::istream &in, int &open_parenthesis_to_match);

void consume_word(std::istream &in, const std::string &expectation, int &open_parenthesis_to_match);

void consume(std::istream &in, int expectation, int &open_parenthesis_to_match);

void skip_whitespaces(std::istream &in, int &open_parenthesis_to_match);

std::string consume_and_find_next_keyword(std::istream &in, int &open_parenthesis_to_match);


#endif // PARSE_H
