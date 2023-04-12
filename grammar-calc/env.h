#ifndef ENV_H
#define ENV_H


#pragma once

#include "pointer.h"
#include <string>

class Val;

CLASS(Env) {
public:
    static PTR(Env) empty;
    virtual PTR(Val) lookup(std::string find_name) = 0;
    virtual ~Env() = default;
};

class EmptyEnv : public Env {
public:
    EmptyEnv() = default;

    PTR(Val) lookup(std::string matcher);
};

class ExtendedEnv : public Env {
public:
    std::string name;
    PTR(Val) val;
    PTR(Env) rest;

    ExtendedEnv(std::string name, PTR(Val) val, PTR(Env) rest);

    PTR(Val) lookup(std::string matcher);
};

#endif // ENV_H
