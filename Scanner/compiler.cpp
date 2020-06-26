//#include <iostream>
//#include <ostream>
//#include "Scanner.h"
//#include <fstream>
//
//int main() {
//
//	std::ifstream ifile("my.txt");
//	Scanner scanner(ifile);
//
//	for (;;) {
//		Token currentLexem = scanner.getNextToken();
//		currentLexem.print(std::cout);
//
//		if (currentLexem.type() == LexemType::error || currentLexem.type() == LexemType::eof) {
//			break;
//		}
//	}
//
//	return 1;
//};