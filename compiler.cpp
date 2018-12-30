/* сщьзшдук.cpp */

/* Язык программирования slang, компилятор в байт-код
 * Лицензия: GNU GPL v3
 * Автор: Кручинин Тимофей <tymcrit.gmail.com>
 * Данное програмное обеспечение распространяется КАК ЕСТЬ и не предоставляет никаких гарантий
 */

#include "main.hpp"

// Функция delspaces приводит строку в приличный вид - заменяет все табы пробелами, удаляет все двойные пробелы, удаляет все пробелы из начала строки и добавляет один пробел в конец, если его там не было
void delSpaces(std::string & s) {
    while (s.find("\t") != std::string().npos) {
        s.replace(s.begin() + s.find("\t"), s.begin() + s.find("\t") + 1, " ");
    }
    while (s.find("  ") != std::string().npos) {
        s.erase(s.begin() + s.find("  "), s.begin() + s.find("  ") + 1);
    }
    if (!s.empty()) {
        if (s.substr(0, 1) == " ") {
            s.erase(s.begin(), s.begin() + 1);
        }
        if (s.substr(s.size() - 1, 1) != " ") {
            s += " ";
        }
    }
}

// Получить первое слово из приведённой в порядок строки
std::string getWord(const std::string & s) {
      return s.substr(0, s.find(" "));
}

// Удалить первое слово из приведённой в порядок строки
void delWord(std::string & s) {
    s.erase(s.begin(), s.begin() + s.find(" ") + 1);
}

// Выдать ощибку и аварийно завершиться
void error(int line, int code, std::string msg) {
    std::string errArr[] = {"", "unknown command", "variable can't be a number", "unknown variable", "too few arguments", "to more arguments", "expected variable, found", "expected comparsion operator, found", "unbracket end", "else without if"};
    std::ostringstream err;
    err << "Line " << line << ": error: " << errArr[code] << " " << msg << std::endl;
    throw err.str();
}


// Проверка строки на число
bool isNumber(std::string s) {
    return std::string("0123456789-").find(s.at(0)) != std::string().npos && s.substr(1).find_first_not_of("0123456789") == std::string().npos;
}

// ПОлучить код переменной из строки
int getVar(std::string & s, std::map<std::string, int> v, int line) {
    if (s.empty()) {
        error(line, 4);
    }
    std::string w = getWord(s);
    delWord(s);
    if (v.find(w) == v.end()) {
        error(line, 3, w);
    }
    return v[w];
}

int addConst(std::map<std::string, int> & vars, std::vector<std::pair<int, int>> & consts, int val) {
    consts.push_back(std::pair<int, int>(vars.size(), val));
    vars[std::to_string(vars.size())] = vars.size();
    return vars.size() - 1;
}

// Получить код переменную или число из строки
int getCom(std::string & s, std::map<std::string, int> & v, int line, std::vector<std::pair<int, int>> & consts) {
    if (s.empty()) {
        error(line, 4);
    }
    std::string w = getWord(s);
    delWord(s);
    if (!isNumber(w) && v.find(w) == v.end()) {
        error(line, 6, w);
    }
    if (isNumber(w)) {
        return addConst(v, consts, stoi(w));
    }
    return v[w];
}

// Получить код операции из строки
int getOp(std::string & s, std::map<std::string, int> v, int line) {
    const std::map<std::string, int> mp = {
        std::pair<std::string, int>("<", 0),
        std::pair<std::string, int>("<=", 1),
        std::pair<std::string, int>("=", 2),
        std::pair<std::string, int>(">=", 3),
        std::pair<std::string, int>(">", 4),
        std::pair<std::string, int>("<>", 5)
    };
    if (s.empty()) {
        error(line, 4);
    }
    std::string w = getWord(s);
    delWord(s);
    if (mp.find(w) == mp.end()) {
        error(line, 7, w);
    }
    return mp.at(w);
}

// Получить переменную для инициализации
void getInit(std::string & s, std::map<std::string, int> & vars, int line) {
    std::string var = getWord(s);
    delWord(s);
    if (isNumber(var)) {
        error(line, 2);
    }
    vars[var] = vars.size();
}

// Найти парный блочный оператор
int findBracket(const std::vector<int> & brackets, const int & ifDepth, const int & whileDepth, int index, bool & flag) {
    int it = ifDepth + 1, wt = whileDepth + 1;
    while (it != ifDepth && wt != whileDepth && index > 0) {
        index--;
        switch (brackets[index]) {
        case 1:
            wt--;
            break;
        case 2:
            it--;
            break;
        case 3:
            it++;
            break;
        case 4:
            wt++;
            break;
        case 5:
            if (it == ifDepth + 1) {
                it = ifDepth;
            }
            break;
        }
    }
    if (it != ifDepth && wt != whileDepth && index == 0) {
        error(-1, 8);
    }
    flag = it == ifDepth;
    return index;
}

// Проверить является ли на данном уровне вложенности самым глубоким блоком if ... else или if ... end
int ifDepthest(std::vector<int> brackets, int index) {
    int id = 1, wd = 0;
    for (int i = index; i >= 0; i--) {
        switch (brackets[i]) {
        case 1:
            wd--;
            break;
        case 2:
            id--;
            if (id == 0 && wd == 0) {
                return i;
            }
            if (id == 0 && wd != 0) {
                return -1;
            }
            break;
        case 3:
            id++;
            break;
        case 4:
            wd++;
            break;
        case 5:
            if (id == 0) {
                return -1;
            }
            break;
        }
    }
    return -1;
}

// Скомлировать очередную строку программы
void compile(std::string s, std::vector<std::pair<int, std::vector<int>>> & comp, std::vector<std::pair<int, int>> & consts) {
    static int line, ifDepth, whileDepth;
    const int commandIndex = comp.size();
    static std::map<std::string, int> vars;
    static std::vector<int> jumps = {0};
    line++;
    jumps.push_back(0);
    delSpaces(s);
    if (s.empty()) {
        return;
    }
    std::string command = getWord(s);
    delWord(s);
    const std::map<std::string, int> commands = {                                         // Список всех команд
        std::pair<std::string, int>("var", -1),
        std::pair<std::string, int>("#", -1),
        std::pair<std::string, int>("set", 0),
        std::pair<std::string, int>("+", 2),
        std::pair<std::string, int>("-", 3),
        std::pair<std::string, int>("*", 4),
        std::pair<std::string, int>("/", 5),
        std::pair<std::string, int>("%", 6),
        std::pair<std::string, int>("arrset", 7),
        std::pair<std::string, int>("arrget", 8),
        std::pair<std::string, int>("while", 9),
        std::pair<std::string, int>("if", 9),
        std::pair<std::string, int>("end", 10),
        std::pair<std::string, int>("else", 10),
        std::pair<std::string, int>("in", 12),
        std::pair<std::string, int>("out", 13),
        std::pair<std::string, int>("read", 14),
        std::pair<std::string, int>("print", 15)
    };
    const std::set<std::string> varcom = {"arrget", "*", "/", "%"};                       // Список команд, параметрами которых являются переменная и значение
    const std::set<std::string> comcom = {"arrset"};                                      // Список команд, параметрами которых являются два значения
    const std::set<std::string> block = {"while", "if"};                                  // Список блочных команд
    const std::set<std::string> ariphm = {"set", "+", "-", "*", "/", "%"};                // Список арифсетических операций
    const std::set<std::string> com = {"out", "print"};                                   // Список команд, параметром которых является значение
    const std::set<std::string> var = {"in", "read", "set", "+", "-"};                    // Список команд, параметром которых является переменная
    if(commands.find(command) == commands.end()) {
        error(line, 1, command);
    } else if (command == "#") {
        return;
    } else if (command == "var") {
        while (!s.empty()) {
            getInit(s, vars, line);
        }
    } else if (varcom.find(command) != varcom.end()) {
        int var = getVar(s, vars, line);
        int arg = getCom(s, vars, line, consts);
        if (!s.empty() && ariphm.find(command) != ariphm.end()) {
            int arg2 = getCom(s, vars, line, consts);
            comp.push_back(std::pair<int, std::vector<int>>(commands.at(command), std::vector<int>{var, arg, arg2}));
        } else if (ariphm.find(command) != ariphm.end()) {
            comp.push_back(std::pair<int, std::vector<int>>(commands.at(command), std::vector<int>{var, var, arg}));
        } else {
            comp.push_back(std::pair<int, std::vector<int>>(commands.at(command), std::vector<int>{var, arg}));
        }
    } else if (comcom.find(command) != comcom.end()) {
        int index = getCom(s, vars, line, consts);
        int element = getCom(s, vars, line, consts);
        comp.push_back(std::pair<int, std::vector<int>>(commands.at(command), std::vector<int>{index, element}));
    } else if (block.find(command) != block.end()) {
        int paramOne = getCom(s, vars, line, consts);
        int comparsion = getOp(s, vars, line);
        int paramTwo = getCom(s, vars, line, consts);
        comp.push_back(std::pair<int, std::vector<int>>(commands.at(command), std::vector<int>{paramOne, paramTwo, comparsion, -1}));
        jumps.resize(commandIndex + 1);
        if (command == "while") {
            jumps[commandIndex] = 1;
            whileDepth++;
        } else if (command == "if") {
            jumps[commandIndex] = 2;
            ifDepth++;
        }
    } else if (command == "end") {
        if (ifDepth + whileDepth == 0) {
            error(line, 8);
        }
        bool flag;
        int index = findBracket(jumps, ifDepth, whileDepth, commandIndex, flag);
        if (jumps[index] == 5) {
            comp[index].second[0] = commandIndex + 1;
        } else {
            comp[index].second[3] = commandIndex + 1;
        }
        if (flag) {
            comp.push_back(std::pair<int, std::vector<int>>(commands.at(command), std::vector<int>{commandIndex + 1}));
            jumps[commandIndex] = 3;
            ifDepth--;
        } else {
            comp.push_back(std::pair<int, std::vector<int>>(commands.at(command), std::vector<int>{index}));
            jumps[commandIndex] = 4;
            whileDepth--;
        }
    } else if (com.find(command) != com.end()) {
        int param = getCom(s, vars, line, consts);
        comp.push_back(std::pair<int, std::vector<int>>(commands.at(command), std::vector<int>{param}));
    } else if (command == "else") {
        int index;
        if ((index = ifDepthest(jumps, commandIndex)) == -1) {
            error(line, 11);
        }
        comp[index].second[3] = commandIndex + 1;
        comp.push_back(std::pair<int, std::vector<int>>(commands.at(command), std::vector<int>{-1}));
        jumps[commandIndex] = 5;
    } else if (var.find(command) != var.end()) {
        int arg1 = getVar(s, vars, line);
        if (!s.empty() && ariphm.find(command) != ariphm.end()) {
            int arg2 = getCom(s, vars, line, consts);
            if (!s.empty()) {
                int arg3 = getCom(s, vars, line, consts);
                comp.push_back(std::pair<int, std::vector<int>>(commands.at(command), std::vector<int>{arg1, arg2, arg3}));
            } else {
                if (command == "set") {
                    comp.push_back(std::pair<int, std::vector<int>>(commands.at(command), std::vector<int>{arg1, arg2}));
                } else {
                    comp.push_back(std::pair<int, std::vector<int>>(commands.at(command), std::vector<int>{arg1, arg1, arg2}));
                }
            }
        } else {
            if (command == "set") {
                comp.push_back(std::pair<int, std::vector<int>>(commands.at(command), std::vector<int>{arg1, addConst(vars, consts, 0)}));
            } else if (ariphm.find(command) != ariphm.end()) {
                comp.push_back(std::pair<int, std::vector<int>>(commands.at(command), std::vector<int>{arg1, arg1, addConst(vars, consts, 1)}));
            } else {
                comp.push_back(std::pair<int, std::vector<int>>(commands.at(command), std::vector<int>{arg1}));
            }
        }
    }
    if (!s.empty()) {
        error(line, 5);
    }
}
