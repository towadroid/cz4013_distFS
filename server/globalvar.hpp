//
// Created by Tobias on 23.02.2020.
//

#ifndef SERVER_GLOBALVAR_HPP
#define SERVER_GLOBALVAR_HPP

#include <string>
#include <map>

extern int verbose_flag; //flag for verbose output
extern int semantic; //invocation sematics

const int BACKLOG = 10; // how many pending connections queue will hold

const std::string IEEE754_NONCOMPLIANCE("Not IEC559/IEEE754 compliant, cannot (un-)marshal data");

#endif //SERVER_GLOBALVAR_HPP
