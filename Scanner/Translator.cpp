#include "Translator.h"
#include <algorithm>

Translator::Translator(std::istream &stream) : _scanner(stream) {};

void Translator::printAtoms(std::ostream &stream) {
    std::map < Scope, std::vector < std::shared_ptr < Atom >> > ::iterator
    it = _atoms.begin();
    std::cout << "SYNTAX OK\n";
    while (it != _atoms.end()) {
        std::vector < std::shared_ptr < Atom >> ::iterator
        iter = it->second.begin();
        int n = 0;
        while (iter != it->second.end()) {
            stream << it->first << "\t" << it->second[n]->toString() << std::endl;
            n++;
            ++iter;
        }
        ++it;
    }
}

void Translator::generateAtom(std::shared_ptr <Atom> atom, Scope scope) {
    _atoms[scope].push_back(atom);
}

std::shared_ptr <LabelOperand> Translator::newLabel() {
    _lbl_counter += 1;
    return std::make_shared<LabelOperand>(LabelOperand(_lbl_counter));
}

void Translator::syntaxError(const std::string &message) {
    try {
        throw 1;
    }
    catch (int a) {
        std::cout << message;
        exit(1);
    }
}

void Translator::lexicalError(const std::string &message) {
    try {
        throw 1;
    }
    catch (int a) {
        std::cout << message;
        exit(1);
    }
}

std::shared_ptr <RValue> Translator::Einit(Scope scope) {
    return E(scope);
}

std::shared_ptr <RValue> Translator::E(Scope scope) {
    return E7(scope);
}

std::shared_ptr <RValue> Translator::E7(Scope scope) {
    auto r = E6(scope);
    auto s = E7_(scope, r);
    return s;
}

std::shared_ptr <RValue> Translator::E7_(Scope scope, std::shared_ptr <RValue> p) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::opor) {
        auto r = E6(scope);
        auto s = _SymbolTable.alloc(scope);
        generateAtom(std::make_shared<BinaryOpAtom>("OR", p, r, s), scope);
        auto t = E7_(scope, s);
        return t;
    }

    _scanner.toggle();
    return p;
}

std::shared_ptr <RValue> Translator::E6(Scope scope) {
    auto r = E5(scope);
    auto s = E6_(scope, r);
    return s;
}

std::shared_ptr <RValue> Translator::E6_(Scope scope, std::shared_ptr <RValue> p) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::opand) {
        auto r = E5(scope);
        auto s = _SymbolTable.alloc(scope);
        generateAtom(std::make_shared<BinaryOpAtom>("AND", p, r, s), scope);
        auto t = E6_(scope, s);
        return t;
    }

    _scanner.toggle();
    return p;
}


std::shared_ptr <RValue> Translator::E5(Scope scope) {
    auto r = E4(scope);
    auto s = E5_(scope, r);
    return s;
}

std::shared_ptr <RValue> Translator::E5_(Scope scope, std::shared_ptr <RValue> p) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::opeq) {
        auto r = E4(scope);
        auto s = _SymbolTable.alloc(scope);
        auto l = newLabel();
        generateAtom(std::make_shared<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(1), s), scope);
        generateAtom(std::make_shared<ConditionalJumpAtom>("EQ", p, r, l), scope);
        generateAtom(std::make_shared<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(0), s), scope);
        generateAtom(std::make_shared<LabelAtom>(l), scope);
        return s;
    }

    if (_currentLexem.type() == LexemType::opne) {
        auto r = E4(scope);
        auto s = _SymbolTable.alloc(scope);
        auto l = newLabel();
        generateAtom(std::make_shared<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(1), s), scope);
        generateAtom(std::make_shared<ConditionalJumpAtom>("NE", p, r, l), scope);
        generateAtom(std::make_shared<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(0), s), scope);
        generateAtom(std::make_shared<LabelAtom>(l), scope);
        return s;
    }

    if (_currentLexem.type() == LexemType::opgt) {
        auto r = E4(scope);
        auto s = _SymbolTable.alloc(scope);
        auto l = newLabel();
        generateAtom(std::make_shared<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(1), s), scope);
        generateAtom(std::make_shared<ConditionalJumpAtom>("GT", p, r, l), scope);
        generateAtom(std::make_shared<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(0), s), scope);
        generateAtom(std::make_shared<LabelAtom>(l), scope);
        return s;
    }

    if (_currentLexem.type() == LexemType::oplt) {
        auto r = E4(scope);
        auto s = _SymbolTable.alloc(scope);
        auto l = newLabel();
        generateAtom(std::make_shared<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(1), s), scope);
        generateAtom(std::make_shared<ConditionalJumpAtom>("LT", p, r, l), scope);
        generateAtom(std::make_shared<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(0), s), scope);
        generateAtom(std::make_shared<LabelAtom>(l), scope);
        return s;
    }

    if (_currentLexem.type() == LexemType::ople) {
        auto r = E4(scope);
        auto s = _SymbolTable.alloc(scope);
        auto l = newLabel();
        generateAtom(std::make_shared<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(1), s), scope);
        generateAtom(std::make_shared<ConditionalJumpAtom>("LE", p, r, l), scope);
        generateAtom(std::make_shared<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(0), s), scope);
        generateAtom(std::make_shared<LabelAtom>(l), scope);
        return s;
    }

    _scanner.toggle();
    return p;
}


std::shared_ptr <RValue> Translator::E4(Scope scope) {

    auto r = E3(scope);
    auto s = E4_(scope, r);
    return s;
}

std::shared_ptr <RValue> Translator::E4_(Scope scope, std::shared_ptr <RValue> p) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if ((_currentLexem.type() == LexemType::opplus)) {
        auto r = E3(scope);
        auto s = _SymbolTable.alloc(scope);
        generateAtom(std::make_shared<BinaryOpAtom>("ADD", p, r, s), scope);
        auto t = E4_(scope, s);
        return t;
    }

    if (_currentLexem.type() == LexemType::opminus) {
        auto r = E3(scope);
        auto s = _SymbolTable.alloc(scope);
        generateAtom(std::make_shared<BinaryOpAtom>("SUB", p, r, s), scope);
        auto t = E4_(scope, s);
        return t;
    }

    _scanner.toggle();
    return p;
}

std::shared_ptr <RValue> Translator::E3(Scope scope) {

    auto r = E2(scope);
    auto s = E3_(scope, r);
    return s;
}

std::shared_ptr <RValue> Translator::E3_(Scope scope, std::shared_ptr <RValue> p) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if ((_currentLexem.type() == LexemType::opmult)) {
        auto r = E2(scope);
        auto s = _SymbolTable.alloc(scope);
        generateAtom(std::make_shared<BinaryOpAtom>("MUL", p, r, s), scope);
        auto t = E3_(scope, s);
        return t;
    }

    _scanner.toggle();
    return p;
}

std::shared_ptr <RValue> Translator::E2(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if ((_currentLexem.type() == LexemType::opnot)) {
        auto q = E1(scope);
        auto r = _SymbolTable.alloc(scope);
        generateAtom(std::make_shared<UnaryOpAtom>("NOT", q, r), scope);
        return r;
    }

    _scanner.toggle();
    return E1(scope);
}

std::shared_ptr <RValue> Translator::E1(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if ((_currentLexem.type() == LexemType::lpar)) {
        auto q = E(scope);
        {
            _currentLexem = _scanner.getNextToken();
            if (_currentLexem.type() == LexemType::error) {
                lexicalError("Lexical error");
            }
            if ((_currentLexem.type() == LexemType::rpar)) {
                return q;
            }
            syntaxError("expected ) after (");
            return nullptr;
        }
    }

    if ((_currentLexem.type() == LexemType::num)) {
        return std::make_shared<NumberOperand>(NumberOperand(_currentLexem.value()));
    }

    if ((_currentLexem.type() == LexemType::chr)) {
        return std::make_shared<NumberOperand>(NumberOperand(_currentLexem.value()));
    }

    if ((_currentLexem.type() == LexemType::opinc)) {
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::error) {
            lexicalError("Lexical error");
        }
        if ((_currentLexem.type() == LexemType::id)) {
            auto q = _SymbolTable.checkVar(scope, _currentLexem.str());
            if (!q)
                syntaxError("Syntax error");
            generateAtom(std::make_shared<BinaryOpAtom>("ADD", q, std::make_shared<NumberOperand>(1), q), scope);
            return q;
        }
    }

    if ((_currentLexem.type() == LexemType::id)) {
        auto r = _currentLexem.str();
        return E1_(scope, r);
    }
    syntaxError("expected identifier");
    return nullptr;
}

std::shared_ptr <MemoryOperand> Translator::E1_(Scope scope, std::string p) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::opinc) {
        auto s = _SymbolTable.checkVar(scope, p);
        if (!s)
            syntaxError("Syntax error");
        auto r = _SymbolTable.alloc(scope);
        generateAtom(std::make_shared<UnaryOpAtom>("MOV", s, r), scope);
        generateAtom(std::make_shared<BinaryOpAtom>("ADD", s, std::make_shared<NumberOperand>(1), s), scope);
        return r;
    }

    if (_currentLexem.type() == LexemType::lpar) {
        auto n = ArgList(scope);
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::error) {
            lexicalError("Lexical error");
        }
        if (_currentLexem.type() == LexemType::rpar) {
            auto s = _SymbolTable.checkFunc(p, n);
            if (!s)
                syntaxError("Syntax error");
            auto r = _SymbolTable.alloc(scope);
            generateAtom(std::make_shared<CallAtom>(s, r), scope);
            return r;
        }
        syntaxError("Syntax error");
    }

    _scanner.toggle();
    auto q = _SymbolTable.checkVar(scope, p);
    if (!q)
        syntaxError("Syntax error");
    return q;
}

int Translator::ArgList(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::rpar) {
        _scanner.toggle();
        return 0;
    }

    _scanner.toggle();
    auto p = E(scope);
    auto m = ArgList_(scope);
    generateAtom(std::make_shared<ParamAtom>(p), scope);
    return (m + 1);
}

int Translator::ArgList_(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::comma) {
        auto p = E(scope);
        auto m = ArgList_(scope);
        generateAtom(std::make_shared<ParamAtom>(p), scope);
        return (m + 1);
    }

    _scanner.toggle();
    return 0;
}

bool Translator::DeclareStmt(Scope scope) {

    auto p = Type(scope);
    _currentLexem = _scanner.getNextToken();
    checkForError();
    if (_currentLexem.type() == LexemType::id) {
        DeclareStmt_(scope, p, _currentLexem.str());
        return true;
    }
    syntaxError("Syntax error");
    return false;
}


bool Translator::DeclareStmt_(Scope scope, SymbolTable::TableRecord::RecordType type, std::string name) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::lpar) {
        if (scope > -1) {
            syntaxError("Function difinition inside function");
        }
        auto scope_tmp = _SymbolTable.addFunc(name, type, 0);
        if (scope_tmp == nullptr)
            syntaxError("Syntax error");
        auto scope_ = scope_tmp->index();
        auto n = ParamList(scope_);
        _SymbolTable[scope_]._len = n;

        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::error) {
            lexicalError("Lexical error");
        }
        if (_currentLexem.type() == LexemType::rpar) {
            _currentLexem = _scanner.getNextToken();
            if (_currentLexem.type() == LexemType::error) {
                lexicalError("Lexical error");
            }
            if (_currentLexem.type() == LexemType::lbrace) {
                StmtList(scope_);
                _currentLexem = _scanner.getNextToken();
                if (_currentLexem.type() == LexemType::error) {
                    lexicalError("Lexical error");
                }
                if (_currentLexem.type() == LexemType::rbrace) {
                    generateAtom(std::make_shared<RetAtom>(std::make_shared<NumberOperand>(0)), scope_);
                    return true;
                }
                syntaxError("expected } after {");
            }
            syntaxError("expected ( after )");
        }
        syntaxError("expected ) after (");
    }

    if (_currentLexem.type() == LexemType::opassign) {
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::error) {
            lexicalError("Lexical error");
        }
        if (_currentLexem.type() == LexemType::num) {
            _currentLexem = _scanner.getNextToken();
            if (_currentLexem.type() == LexemType::error) {
                lexicalError("Lexical error");
            }
            if (_currentLexem.type() == LexemType::semicolon) {
                _SymbolTable.addVar(name, scope, type, _currentLexem.value());
                DeclVarList_(scope, type);
                return true;
            }
            syntaxError("expected ; after 'num'");
        }
        syntaxError("expected 'num' after assigning something");
    }

    _SymbolTable.addVar(name, scope, type);
    _scanner.toggle();
    DeclVarList_(scope, type);
    if (_currentLexem.type() == LexemType::semicolon) {
        return true;
    }
    syntaxError("expected semicolon");
    return false;
}

SymbolTable::TableRecord::RecordType Translator::Type(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::kwchar) {
        return SymbolTable::TableRecord::RecordType::chr;
    }

    if (_currentLexem.type() == LexemType::kwint) {
        return SymbolTable::TableRecord::RecordType::integer;
    }
    syntaxError("expected 'chr' or 'int'");
    return SymbolTable::TableRecord::RecordType::unknown;
}


bool Translator::DeclVarList_(Scope scope, SymbolTable::TableRecord::RecordType type) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::comma) {
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::error) {
            lexicalError("Lexical error");
        }
        if (_currentLexem.type() == LexemType::id) {
            auto name = _currentLexem.str();
            InitVar(scope, type, name);
            DeclVarList_(scope, type);
            return true;
        }
        syntaxError("expected identifier");
    }

    _scanner.toggle();
    return true;
}

bool Translator::InitVar(Scope scope, SymbolTable::TableRecord::RecordType type, std::string name) {
    _currentLexem = _scanner.getNextToken();
    checkForError();
    if (_currentLexem.type() == LexemType::opassign) {
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::error) {
            lexicalError("Lexical error");
        }

        if (_currentLexem.type() == LexemType::num) {
            _SymbolTable.addVar(name, scope, type, _currentLexem.value());
            return true;
        }

        if (_currentLexem.type() == LexemType::chr) {
            _SymbolTable.addVar(name, scope, type, _currentLexem.value());
            return true;
        }
        syntaxError("expected 'num' | 'chr' when initializing an identifier");
    }

    _scanner.toggle();
    _SymbolTable.addVar(name, scope, type);
    return true;
}


int Translator::ParamList(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::kwchar || _currentLexem.type() == LexemType::kwint) {
        _scanner.toggle();
        auto q = Type(scope);
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::error) {
            lexicalError("Lexical error");
        }
        if (_currentLexem.type() == LexemType::id) {
            auto name = _currentLexem.str();
            _SymbolTable.addVar(name, scope, q);
            auto s = ParamList_(scope);
            return (s + 1);
        }
        syntaxError("expected identifier while enumerating a paramlist");
    }

    _scanner.toggle();
    return 0;
}

int Translator::ParamList_(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::comma) {
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::error) {
            lexicalError("Lexical error");
        }
        if (_currentLexem.type() == LexemType::kwchar || _currentLexem.type() == LexemType::kwint) {
            _scanner.toggle();
            auto q = Type(scope);
            _currentLexem = _scanner.getNextToken();
            if (_currentLexem.type() == LexemType::error) {
                lexicalError("Lexical error");
            }
            if (_currentLexem.type() == LexemType::id) {
                auto name = _currentLexem.str();
                _SymbolTable.addVar(name, scope, q);
                auto s = ParamList_(scope);
                return (s + 1);
            }
            syntaxError("expected identifier name while enumerating a paramlist");
        }
        syntaxError("expected type of identifier while enumerating a paramlist");
    }

    _scanner.toggle();
    return 0;
}

bool Translator::StmtList(Scope scope) {
    _currentLexem = _scanner.getNextToken();

    checkForError();

    if (_currentLexem.type() == LexemType::id || _currentLexem.type() == LexemType::kwwhile ||
        _currentLexem.type() == LexemType::kwfor || _currentLexem.type() == LexemType::kwif ||
        _currentLexem.type() == LexemType::kwswitch || _currentLexem.type() == LexemType::kwin ||
        _currentLexem.type() == LexemType::kwout || _currentLexem.type() == LexemType::lbrace ||
        _currentLexem.type() == LexemType::semicolon || _currentLexem.type() == LexemType::kwreturn ||
        _currentLexem.type() == LexemType::kwchar || _currentLexem.type() == LexemType::kwint) {
        _scanner.toggle();
        Stmt(scope);
        StmtList(scope);
        return true;
    }

    _scanner.toggle();
    return true;
}

bool Translator::Stmt(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::kwint || _currentLexem.type() == LexemType::kwchar) {
        _scanner.toggle();
        DeclareStmt(scope);
        return true;
    }

    if (_currentLexem.type() == LexemType::semicolon) {
        return true;
    }
    if (scope == -1)
        syntaxError("Operator should be inside a function");

    if (_currentLexem.type() == LexemType::id) {
        _scanner.toggle();
        AssignOrCallOp(scope);
        return true;
    }

    if (_currentLexem.type() == LexemType::kwwhile) {
        _scanner.toggle();
        WhileOp(scope);
        return true;
    }

    if (_currentLexem.type() == LexemType::kwfor) {
        _scanner.toggle();
        ForOp(scope);
        return true;
    }

    if (_currentLexem.type() == LexemType::kwswitch) {
        _scanner.toggle();
        SwitchOp(scope);
        return true;
    }

    if (_currentLexem.type() == LexemType::kwif) {
        _scanner.toggle();
        IfOp(scope);
        return true;
    }

    if (_currentLexem.type() == LexemType::kwin) {
        _scanner.toggle();
        IOp(scope);
        return true;
    }

    if (_currentLexem.type() == LexemType::kwout) {
        _scanner.toggle();
        OOp(scope);
        return true;
    }

    if (_currentLexem.type() == LexemType::lbrace) {
        StmtList(scope);
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::error) {
            lexicalError("Lexical error");
        }
        if (_currentLexem.type() == LexemType::rbrace) {
            return true;
        }
        syntaxError("expected ) after (");
    }

    if (_currentLexem.type() == LexemType::kwreturn) {
        auto p = E(scope);
        generateAtom(std::make_shared<RetAtom>(p), scope);
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::error) {
            lexicalError("Lexical error");
        }
        if (_currentLexem.type() == LexemType::semicolon) {
            return true;
        }
        syntaxError("expected semicolon after 'return' keyword");
    }
    syntaxError("expected return");
    return false;
}

bool Translator::AssignOrCallOp(Scope scope) {
    AssignOrCall(scope);
    _currentLexem = _scanner.getNextToken();
    checkForError();
    if (_currentLexem.type() == LexemType::semicolon) {
        return true;
    }
    syntaxError("expected semicolon after assigning or calling something");
    return false;
}

bool Translator::AssignOrCall(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::id) {
        auto q = _currentLexem.str();
        AssignOrCall_(scope, q);
        return true;
    }
    syntaxError("expected identifier when assigning or calling something");
    return false;
}

bool Translator::AssignOrCall_(Scope scope, std::string p) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::opassign) {
        auto q = E(scope);
        auto r = _SymbolTable.checkVar(scope, p);
        generateAtom(std::make_shared<UnaryOpAtom>("MOV", q, r), scope);
        return true;
    }

    if (_currentLexem.type() == LexemType::lpar) {
        auto n = ArgList(scope);
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::error) {
            lexicalError("Lexical error");
        }
        if (_currentLexem.type() == LexemType::rpar) {
            auto q = _SymbolTable.checkFunc(p, n);
            auto r = _SymbolTable.alloc(scope);
            generateAtom(std::make_shared<CallAtom>(q, r), scope);
            return true;
        }
        syntaxError("expected ) after ( when calling a function or assigning a value");
    }
    syntaxError("expected ( before calling a function or assigning a value");
    return false;
}

bool Translator::WhileOp(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::kwwhile) {
        auto l1 = newLabel();
        generateAtom(std::make_shared<LabelAtom>(l1), scope);
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::error) {
            lexicalError("Lexical error");
        }
        if (_currentLexem.type() == LexemType::lpar) {
            auto p = E(scope);
            _currentLexem = _scanner.getNextToken();
            if (_currentLexem.type() == LexemType::error) {
                lexicalError("Lexical error");
            }
            if (_currentLexem.type() == LexemType::rpar) {
                auto l2 = newLabel();
                generateAtom(std::make_shared<ConditionalJumpAtom>("EQ", p, std::make_shared<NumberOperand>(0), l2),
                             scope);
                Stmt(scope);
                generateAtom(std::make_shared<JumpAtom>(l1), scope);
                generateAtom(std::make_shared<LabelAtom>(l2), scope);
                return true;
            }
            syntaxError("expected ) after ( after expression in 'while'");
        }
        syntaxError("expected ( after 'while' keyword");
    }
    syntaxError("expected 'while' loop");
    return false;
}

bool Translator::ForOp(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::kwfor) {
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::error) {
            lexicalError("Lexical error");
        }
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::lpar) {
            ForInit(scope);
            _currentLexem = _scanner.getNextToken();
            if (_currentLexem.type() == LexemType::error) {
                lexicalError("Lexical error");
            }
            if (_currentLexem.type() == LexemType::semicolon) {
                auto l1 = newLabel();
                generateAtom(std::make_shared<LabelAtom>(l1), scope);
                auto p = ForExp(scope);
                _currentLexem = _scanner.getNextToken();
                if (_currentLexem.type() == LexemType::error) {
                    lexicalError("Lexical error");
                }
                if (_currentLexem.type() == LexemType::semicolon) {
                    auto l2 = newLabel();
                    auto l3 = newLabel();
                    auto l4 = newLabel();
                    generateAtom(std::make_shared<ConditionalJumpAtom>("EQ", p, std::make_shared<NumberOperand>(0), l4),
                                 scope);
                    generateAtom(std::make_shared<JumpAtom>(l3), scope);
                    generateAtom(std::make_shared<LabelAtom>(l2), scope);
                    ForLoop(scope);
                    generateAtom(std::make_shared<JumpAtom>(l1), scope);
                    _currentLexem = _scanner.getNextToken();
                    if (_currentLexem.type() == LexemType::error) {
                        lexicalError("Lexical error");
                    }
                    if (_currentLexem.type() == LexemType::rpar) {
                        generateAtom(std::make_shared<LabelAtom>(l3), scope);
                        Stmt(scope);
                        generateAtom(std::make_shared<JumpAtom>(l1), scope);
                        generateAtom(std::make_shared<LabelAtom>(l4), scope);
                        return true;
                    }
                    syntaxError("expected ) after ; in for");
                }
                syntaxError("expected semicolon after semicolon in for");
            }
            syntaxError("expected semicolon after ( in for");
        }
        syntaxError("expected ( after for keyword");
    }
    syntaxError("expected for loop");
    return false;
}

bool Translator::ForInit(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::id) {
        _scanner.toggle();
        AssignOrCallOp(scope);
        return true;
    }

    _scanner.toggle();
    return true;
}

std::shared_ptr <RValue> Translator::ForExp(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::num || _currentLexem.type() == LexemType::chr ||
        _currentLexem.type() == LexemType::id || _currentLexem.type() == LexemType::opinc ||
        _currentLexem.type() == LexemType::lpar) {
        _scanner.toggle();
        auto p = E(scope);
        return p;
    }

    return std::make_shared<NumberOperand>('1');
}

bool Translator::ForLoop(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::id || _currentLexem.type() == LexemType::opassign ||
        _currentLexem.type() == LexemType::lpar) {
        AssignOrCallOp(scope);
        return true;
    }

    if (_currentLexem.type() == LexemType::opinc) {
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::error) {
            lexicalError("Lexical error");
        }
        if (_currentLexem.type() == LexemType::id) {
            auto name = _currentLexem.str();
            auto p = _SymbolTable.checkVar(scope, name);
            generateAtom(std::make_shared<BinaryOpAtom>("ADD", p, std::make_shared<NumberOperand>(1), p), scope);
            return true;
        }
        syntaxError("expected an identificator when using increment");
    }

    _scanner.toggle();
    return true;
}


bool Translator::IfOp(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::kwif) {
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::error) {
            lexicalError("Lexical error");
        }
        if (_currentLexem.type() == LexemType::lpar) {
            auto p = E(scope);
            _currentLexem = _scanner.getNextToken();
            if (_currentLexem.type() == LexemType::error) {
                lexicalError("Lexical error");
            }
            if (_currentLexem.type() == LexemType::rpar) {
                auto l1 = newLabel();
                generateAtom(std::make_shared<ConditionalJumpAtom>("EQ", p, std::make_shared<NumberOperand>(0), l1),
                             scope);
                Stmt(scope);
                auto l2 = newLabel();
                generateAtom(std::make_shared<JumpAtom>(l2), scope);
                generateAtom(std::make_shared<LabelAtom>(l1), scope);
                ElsePart(scope);
                generateAtom(std::make_shared<LabelAtom>(l2), scope);
                return true;
            }
            syntaxError("expected ) after ( in if");
        }
        syntaxError("expected ( after if");
    }

    _scanner.toggle();
    return true;
}

bool Translator::ElsePart(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::kwelse) {
        Stmt(scope);
        return true;
    }

    _scanner.toggle();
    return true;
}


void Translator::Cases_(Scope scope, std::shared_ptr <RValue> p, std::shared_ptr <LabelOperand> end,
                        std::shared_ptr <LabelOperand> def) {
    _currentLexem = _scanner.getNextToken();
    _scanner.toggle();

    auto nulllabel = std::make_shared<LabelOperand>(LabelOperand(0));

    if (_currentLexem.type() == LexemType::kwcase || _currentLexem.type() == LexemType::kwdefault) {
        auto def1 = ACase(scope, p, end);
        Cases_(scope, p, end, std::max(def, def1));
        return;
    }
    std::shared_ptr <LabelOperand> q = def >= nulllabel ? def : end;
    generateAtom(std::make_shared<JumpAtom>(JumpAtom(q)), scope);
    return;
}

bool Translator::Cases(Scope scope, std::shared_ptr <RValue> p, std::shared_ptr <LabelOperand> end) {
    auto def1 = ACase(scope, p, end);
    Cases_(scope, p, end, def1);
    return true;
}


std::shared_ptr <LabelOperand>
Translator::ACase(Scope scope, std::shared_ptr <RValue> p, std::shared_ptr <LabelOperand> end) {
    _currentLexem = _scanner.getNextToken();

    if (_currentLexem.type() == LexemType::kwcase) {
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() != LexemType::num) {
            syntaxError("expected num at ACase");
        }
        int val = _currentLexem.value();
        auto next = newLabel();
        generateAtom(std::make_shared<ConditionalJumpAtom>(
                ConditionalJumpAtom("NE", p, std::make_shared<NumberOperand>(NumberOperand(val)), next)), scope);
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() != LexemType::colon) {
            syntaxError("expected : at ACase");
        }
        Stmt(scope);
        generateAtom(std::make_shared<LabelAtom>(LabelAtom(end)), scope);
        generateAtom(std::make_shared<LabelAtom>(LabelAtom(next)), scope);
        return std::make_shared<LabelOperand>(LabelOperand(-1));
    }
    if (_currentLexem.type() == LexemType::kwdefault) {
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() != LexemType::colon) {
            syntaxError("expected : at ACase");
        }
        auto next = newLabel();
        generateAtom(std::make_shared<JumpAtom>(JumpAtom(next)), scope);
        auto def = newLabel();
        generateAtom(std::make_shared<LabelAtom>(LabelAtom(def)), scope);
        Stmt(scope);
        generateAtom(std::make_shared<JumpAtom>(JumpAtom(end)), scope);
        generateAtom(std::make_shared<LabelAtom>(LabelAtom(next)), scope);
        return def;
    }
    syntaxError("expected case default at ACase");
}

bool Translator::SwitchOp(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::kwswitch) {
        _currentLexem = _scanner.getNextToken();
        checkForError();

        if (_currentLexem.type() == LexemType::lpar) {
            auto p = E(scope);
            _currentLexem = _scanner.getNextToken();
            checkForError();

            if (_currentLexem.type() == LexemType::rpar) {
                _currentLexem = _scanner.getNextToken();
                checkForError();

                if (_currentLexem.type() == LexemType::lbrace) {
                    auto end = newLabel();
                    Cases(scope, p, end);

                    _currentLexem = _scanner.getNextToken();
                    checkForError();
                    if (_currentLexem.type() == LexemType::rbrace) {
                        generateAtom(std::make_shared<LabelAtom>(LabelAtom(end)), scope);
                        return true;
                    }
                }
                syntaxError("expected { after switch expression");
            }
            syntaxError("expected ( after switch");
        }
    }

    syntaxError("Syntax error");
    return false;
}


bool Translator::IOp(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::kwin) {
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::error) {
            lexicalError("Lexical error");
        }
        if (_currentLexem.type() == LexemType::id) {
            auto name = _currentLexem.str();
            _currentLexem = _scanner.getNextToken();
            if (_currentLexem.type() == LexemType::error) {
                lexicalError("Lexical error");
            }
            if (_currentLexem.type() == LexemType::semicolon) {
                auto p = _SymbolTable.checkVar(scope, name);
                generateAtom(std::make_shared<InAtom>(p), scope);
                return true;
            }
            syntaxError("expected semicolon after identificator in IOp");
        }
        syntaxError("expected an identificator after 'in' keyword");
    }
    syntaxError("expected 'in' in IOp");
    return false;
}

bool Translator::OOp(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::kwout) {
        OOp_(scope);
        _currentLexem = _scanner.getNextToken();
        if (_currentLexem.type() == LexemType::error) {
            lexicalError("Lexical error");
        }
        if (_currentLexem.type() == LexemType::semicolon) {
            return true;
        }
        syntaxError("expected 'semicolon' after 'out' keyword");
    }
    syntaxError("expected 'out' in OOp");
    return false;
}


bool Translator::OOp_(Scope scope) {
    _currentLexem = _scanner.getNextToken();
    checkForError();

    if (_currentLexem.type() == LexemType::num || _currentLexem.type() == LexemType::chr ||
        _currentLexem.type() == LexemType::id || _currentLexem.type() == LexemType::opinc ||
        _currentLexem.type() == LexemType::lpar) {
        _scanner.toggle();
        auto p = E(scope);
        generateAtom(std::make_shared<OutAtom>(p), scope);
        return true;
    }

    if (_currentLexem.type() == LexemType::str) {
        auto s = _StringTable.add(_currentLexem.str());
        generateAtom(std::make_shared<OutAtom>(s), scope);
        return true;
    }
    syntaxError("expected 'str' | 'num' | 'chr' | 'id' | 'inc' | 'lpar' | in OOp_");
    return false;
}

bool Translator::translate() {
    StmtList(GlobalScope);
    return true;
}

void Translator::printST() {
    _SymbolTable.printST();
}

void Translator::printStrT() {
    std::cout << _StringTable;
}

void Translator::checkForError() {
    if (_currentLexem.type() == LexemType::error) {
        lexicalError("Lexical error");
    }
}