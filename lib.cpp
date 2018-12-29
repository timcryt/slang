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
    int kolvar = analisys(compiled);
    run(compiled, kolvar, i, o);
    return o.str();
}

