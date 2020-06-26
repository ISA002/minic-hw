#pragma once

#include "LexicalAnalyser.h"
#include <iostream>
#include <ostream>
#include <unordered_map>
#include <map>
#include <string>
#include <typeinfo>


class Token {
private:
    LexemType _type;
    int _value;
    std::string _str;
    bool ForInt = false;

public:
    std::map <LexemType, std::string> lexemTranslations{
            {LexemType::num,       "num"},
            {LexemType::chr,       "chr"},
            {LexemType::str,       "str"},
            {LexemType::id,        "id"},
            {LexemType::lpar,      "lpar"},
            {LexemType::rpar,      "rpar"},
            {LexemType::lbrace,    "lbrace"},
            {LexemType::rbrace,    "rbrace"},
            {LexemType::lbracket,  "lbracket"},
            {LexemType::rbracket,  "rbracket"},
            {LexemType::semicolon, "semicolon"},
            {LexemType::comma,     "comma"},
            {LexemType::colon,     "colon"},
            {LexemType::opassign,  "opassign"},
            {LexemType::opplus,    "opplus"},
            {LexemType::opminus,   "opminus"},
            {LexemType::opmult,    "opmult"},
            {LexemType::opinc,     "opinc"},
            {LexemType::opeq,      "opeq"},
            {LexemType::opne,      "opne"},
            {LexemType::oplt,      "oplt"},
            {LexemType::opgt,      "opgt"},
            {LexemType::ople,      "ople"},
            {LexemType::opnot,     "opnot"},
            {LexemType::opor,      "opor"},
            {LexemType::opand,     "opand"},
            {LexemType::kwint,     "kwint"},
            {LexemType::kwchar,    "kwchar"},
            {LexemType::kwif,      "kwif"},
            {LexemType::kwelse,    "kwelse"},
            {LexemType::kwswitch,  "kwswitch"},
            {LexemType::kwcase,    "kwcase"},
            {LexemType::kwwhile,   "kwwhile"},
            {LexemType::kwfor,     "kwfor"},
            {LexemType::kwreturn,  "kwreturn"},
            {LexemType::kwdefault, "kwdefault"},
            {LexemType::kwin,      "kwin"},
            {LexemType::kwout,     "kwout"},
            {LexemType::eof,       "eof"},
            {LexemType::error,     "error"}};

    Token() = default;

    Token(LexemType type) : _type(type) {};

    Token(LexemType type, int value) : _type(type), _value(value) { ForInt = true; };

    Token(LexemType type, const std::string &str) : _type(type), _str(str) {};

    int value() { return _value; };

    std::string str() { return _str; };

    LexemType type() { return _type; };

    void print(std::ostream &stream) {
        if (_type == LexemType::eof) {
            stream << "[" + lexemTranslations[_type] + ']' << std::endl;
        } else if (_type == LexemType::error) {
            stream << '[' + lexemTranslations[_type] + ':' + _str + ']' << std::endl;
        } else if (this->_str.empty() == false) {
            stream << '[' + lexemTranslations[_type] + ", " + "\"" + _str + +"\"" + ']' << std::endl;
        } else if (ForInt) {
            stream << '[' + lexemTranslations[_type] + ", " << _value << ']' << std::endl;
        } else {
            stream << '[' + lexemTranslations[_type] + ']' << std::endl;
        }
    }
};

class Scanner {
private:
    char SavedChar;
    std::istream &LexemStream;
    int State = 0;
    int Value;
    std::string value;
    int GoTo = 0;
    bool isWork = false;
    Token _lastToken;

public:
    Scanner(std::istream &stream) : LexemStream(stream) {};

    void toggle() {
        isWork = true;
    }

    Token getNextToken() {
        if (isWork) {
            isWork = false;
            return _lastToken;
        }

        while (true) {
            if (this->GoTo == 0) {
                LexemStream >> std::noskipws >> this->SavedChar;
            } else { this->GoTo = 0; }

            switch (this->State) {
                case 0:

                    if (this->LexemStream.eof()) {
                        _lastToken = Token(LexemType::eof);
                        return Token(LexemType::eof);
                    };
                    // Punct
                    if (SavedChar == '(') {
                        _lastToken = Token(LexemType::lpar);
                        return _lastToken;
                    };
                    if (SavedChar == ')') {
                        _lastToken = Token(LexemType::rpar);
                        return _lastToken;
                    };
                    if (SavedChar == '{') {
                        _lastToken = Token(LexemType::lbrace);
                        return _lastToken;
                    };
                    if (SavedChar == '}') {
                        _lastToken = Token(LexemType::rbrace);
                        return _lastToken;
                    };
                    if (SavedChar == '[') {
                        _lastToken = Token(LexemType::lbracket);
                        return _lastToken;
                    };
                    if (SavedChar == ']') {
                        _lastToken = Token(LexemType::rbracket);
                        return _lastToken;
                    };
                    if (SavedChar == ';') {
                        _lastToken = Token(LexemType::semicolon);
                        return _lastToken;
                    };
                    if (SavedChar == ',') {
                        _lastToken = Token(LexemType::comma);
                        return _lastToken;
                    };
                    if (SavedChar == ':') {
                        _lastToken = Token(LexemType::colon);
                        return _lastToken;
                    };

                    // Operations
                    if (SavedChar == '*') {
                        _lastToken = Token(LexemType::opmult);
                        return _lastToken;
                    };
                    if (SavedChar == '$') {
                        _lastToken = Token(LexemType::eof);
                        return _lastToken;
                    };
                    if (SavedChar == '>') {
                        _lastToken = Token(LexemType::opgt);
                        return _lastToken;
                    };

                    // WS
                    if (SavedChar == ' ') {
                        this->State = 0;
                        break;
                    };
                    if (SavedChar == '\n') {
                        this->State = 0;
                        break;
                    };
                    if (SavedChar == '\t') {
                        this->State = 0;
                        break;
                    };

                    if (SavedChar >= '0' && SavedChar <= '9') {
                        this->Value = (int) SavedChar % 48;
                        this->State = 1;
                        break;
                    };

                    if (SavedChar == '\'') {
                        this->value = "";
                        this->State = 2;
                        break;
                    };

                    if (SavedChar == '"') {
                        this->value = "";
                        this->State = 4;
                        break;
                    };

                    if (SavedChar >= (char) 'a' && SavedChar <= (char) 'z' ||
                        SavedChar >= (char) 'A' && SavedChar <= (char) 'Z') {
                        this->value = SavedChar;
                        this->State = 5;
                        break;
                    };

                    if (SavedChar == '-') {
                        this->State = 6;
                        break;
                    };

                    if (SavedChar == '!') {
                        this->State = 7;
                        break;
                    };

                    if (SavedChar == '<') {
                        this->State = 8;
                        break;
                    };

                    if (SavedChar == '=') {
                        this->State = 9;
                        break;
                    };

                    if (SavedChar == '+') {
                        this->State = 10;
                        break;
                    };

                    if (SavedChar == '|') {
                        this->State = 11;
                        break;
                    };

                    if (SavedChar == '&') {
                        this->State = 12;
                        break;

                    };
                    _lastToken = Token(LexemType::error, "Unsupported symbol");
                    return Token(LexemType::error, "Unsupported symbol");

                case 1:
                    if (SavedChar >= '0' && SavedChar <= '9') {
                        State = 1;
                        if (Value < 0) {
                            Value = Value * 10 - (int) SavedChar % 48;
                            break;
                        } else {
                            Value = Value * 10 + (int) SavedChar % 48;
                            break;
                        }
                    };
                    this->State = 0;
                    this->GoTo = 1;
                    _lastToken = Token(LexemType::num, this->Value);
                    return Token(LexemType::num, this->Value);

                case 2:
                    if (SavedChar == '\'') {
                        _lastToken = Token(LexemType::error, "Empty character constant");
                        return Token(LexemType::error, "Empty character constant");
                    };
                    this->value = SavedChar;
                    this->State = 3;
                    break;

                case 3:
                    if (SavedChar == '\'') {
                        this->State = 0;
                        _lastToken = Token(LexemType::chr, this->value);
                        return Token(LexemType::chr, this->value);
                    };
                    _lastToken = Token(LexemType::error, "Character constant containing more than one character");
                    return Token(LexemType::error, "Character constant containing more than one character");


                case 4:
                    if (SavedChar == '$') {
                        _lastToken = Token(LexemType::error, "Unsupported character");
                        return Token(LexemType::error, "Unsupported character");
                    };
                    if (SavedChar == '"') {
                        this->State = 0;
                        _lastToken = Token(LexemType::str, this->value);
                        return Token(LexemType::str, this->value);
                    };
                    this->State = 4;
                    this->value += SavedChar;
                    break;

                case 5:
                    if (SavedChar >= (char) 'a' && SavedChar <= (char) 'z' ||
                        SavedChar >= (char) 'A' && SavedChar <= (char) 'Z') {
                        this->State = 5;
                        this->value += SavedChar;
                        break;
                    };

                    if (SavedChar >= '0' && SavedChar <= '9') {
                        this->State = 5;
                        this->value += SavedChar;

                        break;
                    };


                    if (this->value == "int") {
                        this->State = 0;
                        this->GoTo = 1;
                        _lastToken = Token(LexemType::kwint);
                        return Token(LexemType::kwint);
                    };
                    if (this->value == "char") {
                        this->State = 0;
                        this->GoTo = 1;
                        _lastToken = Token(LexemType::kwchar);
                        return Token(LexemType::kwchar);
                    };
                    if (this->value == "if") {
                        this->State = 0;
                        this->GoTo = 1;
                        _lastToken = Token(LexemType::kwif);
                        return Token(LexemType::kwif);
                    };
                    if (this->value == "else") {
                        this->State = 0;
                        this->GoTo = 1;
                        _lastToken = Token(LexemType::kwelse);
                        return Token(LexemType::kwelse);
                    };
                    if (this->value == "switch") {
                        this->State = 0;
                        this->GoTo = 1;
                        _lastToken = Token(LexemType::kwswitch);
                        return Token(LexemType::kwswitch);
                    };
                    if (this->value == "case") {
                        this->State = 0;
                        this->GoTo = 1;
                        _lastToken = Token(LexemType::kwcase);
                        return Token(LexemType::kwcase);
                    };
                    if (this->value == "default") {
                        this->State = 0;
                        this->GoTo = 1;
                        _lastToken = Token(LexemType::kwdefault);
                        return Token(LexemType::kwdefault);
                    }
                    if (this->value == "while") {
                        this->State = 0;
                        this->GoTo = 1;
                        _lastToken = Token(LexemType::kwwhile);
                        return Token(LexemType::kwwhile);
                    };
                    if (this->value == "for") {
                        this->State = 0;
                        this->GoTo = 1;
                        _lastToken = Token(LexemType::kwfor);
                        return Token(LexemType::kwfor);
                    };
                    if (this->value == "return") {
                        this->State = 0;
                        this->GoTo = 1;
                        _lastToken = Token(LexemType::kwreturn);
                        return Token(LexemType::kwreturn);
                    };
                    if (this->value == "in") {
                        this->State = 0;
                        this->GoTo = 1;
                        _lastToken = Token(LexemType::kwin);
                        return Token(LexemType::kwin);
                    };
                    if (this->value == "out") {
                        this->State = 0;
                        this->GoTo = 1;
                        _lastToken = Token(LexemType::kwout);
                        return Token(LexemType::kwout);
                    };

                    this->State = 0;
                    this->GoTo = 1;

                    _lastToken = Token(LexemType::id, this->value);
                    return Token(LexemType::id, this->value);

                case 6:
                    this->State = 0;
                    if (SavedChar >= '0' && SavedChar <= '9') {
                        State = 1;
                        this->Value = (-1) * (int) SavedChar % 48;
                        break;
                    };
                    this->GoTo = 1;
                    _lastToken = Token(LexemType::opminus);
                    return Token(LexemType::opminus);


                case 7:
                    this->State = 0;
                    if (SavedChar == '=') {
                        _lastToken = Token(LexemType::opne);
                        return Token(LexemType::opne);
                    };
                    _lastToken = Token(LexemType::opnot);
                    return Token(LexemType::opnot);


                case 8:
                    this->State = 0;
                    if (SavedChar == '=') {
                        _lastToken = Token(LexemType::ople);
                        return Token(LexemType::ople);
                    };
                    _lastToken = Token(LexemType::oplt);
                    return Token(LexemType::oplt);


                case 9:
                    this->State = 0;
                    if (SavedChar == '=') {
                        _lastToken = Token(LexemType::opeq);
                        return Token(LexemType::opeq);
                    };
                    _lastToken = Token(LexemType::opassign);
                    return Token(LexemType::opassign);

                case 10:
                    this->State = 0;
                    if (SavedChar == '+') {
                        _lastToken = Token(LexemType::opinc);
                        return Token(LexemType::opinc);
                    };
                    _lastToken = Token(LexemType::opplus);
                    return Token(LexemType::opplus);


                case 11:
                    if (SavedChar == '|') {
                        this->State = 0;
                        _lastToken = Token(LexemType::opor);
                        return Token(LexemType::opor);
                        break;
                    };
                    _lastToken = Token(LexemType::error, "Incomplete OR operator");
                    return Token(LexemType::error, "Incomplete OR operator");


                case 12:
                    if (SavedChar == '&') {
                        this->State = 0;
                        _lastToken = Token(LexemType::opand);
                        return Token(LexemType::opand);
                        break;
                    };
                    _lastToken = Token(LexemType::error, "Incomplete AND operator");
                    return Token(LexemType::error, "Incomplete AND operator");

            }
        }
    };
};
