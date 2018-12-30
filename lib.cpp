/* lib.hpp */
// Здесь реализована библиотечная функция для встраивания языка в проекты

#include "main.hpp"

std::string runSlang(const std::string & code, const std::string & input) {
    std::istringstream c(code), i(input);
    std::ostringstream o;
    std::vector<std::pair<int, std::vector<int>>> compiled;
    while (!c.eof()) {
        std::string s;
        getline(c, s);
        compile(s, compiled);
    }
    run(compiled, i, o);
    return o.str();
}

