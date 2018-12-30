/* lib.hpp */
// Здесь реализована библиотечная функция для встраивания языка в проекты

#include "main.hpp"

std::string runSlang(const std::string & code, const std::string & input) {
    std::istringstream c(code), i(input);
    std::ostringstream o;
    std::vector<std::pair<int, std::vector<int>>> compiled;
    std::vector<std::pair<int, int>> consts;
    while (!c.eof()) {
        std::string s;
        getline(c, s);
        compile(s, compiled, consts);
    }
    run(compiled, consts, i, o);
    return o.str();
}

