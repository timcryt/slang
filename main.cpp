/* main.cpp */
// Реализация языка в качестве самостоятельной программы

#include <iostream>
#include <fstream>

#include "main.hpp"


int main(int argc, char ** argv) {
    if (argc != 2) {
        std::cerr << "Error: argument error" << std::endl;
        std::cerr << "Using: slang <file>" << std::endl;
        return 1;
    }
    std::ifstream file;
    file.open(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: can't open file" << std::endl;
        return 1;
    }
    std::vector<std::pair<int, std::vector<int>>> compiled;
    while (!file.eof()) {
        std::string s;
        getline(file, s);
        try {
            compile(s, compiled);
        } catch (std::string message) {
            std::cerr << message;
            return 1;
        }
    }
    int kolvar = analisys(compiled);
    run(compiled, kolvar, std::cin, std::cout);
    return 0;
}
