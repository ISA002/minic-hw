#include "StringTable.h"
#include "SymbolTable.h"
#include "Atoms.h"
#include "Translator.h"
#include <iostream>

int main() {
    setlocale(0, "");
    std::ifstream ifile("prog.minic");
    ifile >> std::noskipws;
    Translator transl = Translator(ifile);
    transl.translate();
    transl.printAtoms(std::cout);
    transl.printST();
    transl.printStrT();
    return 0;
}