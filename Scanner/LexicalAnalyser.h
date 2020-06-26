#pragma once

#include <map>
#include <string>

enum class LexemType {
    num, chr, str, id, lpar, rpar, lbrace, rbrace, lbracket, rbracket,
    semicolon, comma, colon, opassign, opplus, opminus, opmult, opinc, opeq, opne, oplt,
    opgt, ople, opnot, opor, opand, kwint, kwchar, kwif, kwelse, kwswitch, kwcase, kwwhile,
    kwfor, kwreturn, kwin, kwout, kwdefault, eof, error
};