/* slang.cpp */

/* Язык программирования slang, версия 1.0 от 29.12.18
 * Лицензия: GNU GPL v3
 * Автор: Кручинин Тимофей <tymcrit.gmail.com>
 * Данное програмное обеспечение распространяется КАК ЕСТЬ и не предоставляет никаких гарантий
 */


#include "main.hpp"
#include <iostream>

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

// Получить код переменную или число из строки
int getCom(std::string & s, std::map<std::string, int> v, int line, bool & f) {
    if (s.empty()) {
        error(line, 4);
    }
    std::string w = getWord(s);
    delWord(s);
    if (!isNumber(w) && v.find(w) == v.end()) {
        error(line, 6, w);
    }
    if (v.find(w) != v.end()) {
        f = true;
        return v[w];
    }
    f = false;
    return stoi(w);
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
void compile(std::string s, std::vector<std::pair<int, std::vector<int>>> & comp) {
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
        std::pair<std::string, int>("+", 7),
        std::pair<std::string, int>("-", 14),
        std::pair<std::string, int>("*", 22),
        std::pair<std::string, int>("/", 29),
        std::pair<std::string, int>("%", 36),
        std::pair<std::string, int>("arrset", 42),
        std::pair<std::string, int>("arrget", 46),
        std::pair<std::string, int>("while", 48),
        std::pair<std::string, int>("if", 48),
        std::pair<std::string, int>("end", 52),
        std::pair<std::string, int>("else", 52),
        std::pair<std::string, int>("in", 54),
        std::pair<std::string, int>("out", 55),
        std::pair<std::string, int>("read", 57),
        std::pair<std::string, int>("print", 58)
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
        bool varFlag;
        int arg = getCom(s, vars, line, varFlag);
        if (!s.empty() && ariphm.find(command) != ariphm.end()) {
            bool argFlag;
            int arg2 = getCom(s, vars, line, argFlag);
            comp.push_back(std::pair<int, std::vector<int>>(commands.at(command) + 2 + (varFlag ? 0 : 2) + (argFlag ? 0 : 1), std::vector<int>{var, arg, arg2}));
        } else {
            comp.push_back(std::pair<int, std::vector<int>>(commands.at(command) + (varFlag ? 0 : 1), std::vector<int>{var, arg}));
        }
    } else if (comcom.find(command) != comcom.end()) {
        bool indexFlag, elementFlag;
        int index = getCom(s, vars, line, indexFlag);
        int element = getCom(s, vars, line, elementFlag);
        comp.push_back(std::pair<int, std::vector<int>>(commands.at(command) + (elementFlag ? 0 : 1) + (indexFlag ? 0 : 2), std::vector<int>{index, element}));
    } else if (block.find(command) != block.end()) {
        bool paramOneFlag, paramTwoFlag;
        int paramOne = getCom(s, vars, line, paramOneFlag);
        int comparsion = getOp(s, vars, line);
        int paraTwo = getCom(s, vars, line, paramTwoFlag);
        comp.push_back(std::pair<int, std::vector<int>>(commands.at(command) + (paramOneFlag ? 0 : 2) + (paramTwoFlag ? 0 : 1), std::vector<int>{paramOne, paraTwo, comparsion, -1}));
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
        bool paramFlag;
        int param = getCom(s, vars, line, paramFlag);
        comp.push_back(std::pair<int, std::vector<int>>(commands.at(command) + (paramFlag ? 0 : 1), std::vector<int>{param}));
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
            bool arg2Flag;
            int arg2 = getCom(s, vars, line, arg2Flag);
            if (!s.empty()) {
                bool arg3Flag;
                int arg3 = getCom(s, vars, line, arg3Flag);
                comp.push_back(std::pair<int, std::vector<int>>(commands.at(command) + 3 + (arg2Flag ? 0 : 2) + (arg3Flag ? 0 : 1), std::vector<int>{arg1, arg2, arg3}));
            } else {
                comp.push_back(std::pair<int, std::vector<int>>(commands.at(command) + 1 + (arg2Flag ? 0 : 1), std::vector<int>{arg1, arg2}));
                
            }
        } else {
            comp.push_back(std::pair<int, std::vector<int>>(commands.at(command), std::vector<int>{arg1}));
        }
    }
    if (!s.empty()) {
        error(line, 5);
    }
}

//Проверить условия условных операторов
bool setFlag(int paramOne, int paramTwo, int compare) {
    switch (compare) {
    case 0:
        return paramOne < paramTwo;
        break;
    case 1:
        return paramOne <= paramTwo;
        break;
    case 2:
        return paramOne == paramTwo;
        break;
    case 3:
        return paramOne >= paramTwo;
        break;
    case 4:
        return paramOne > paramTwo;
        break;
    case 5:
        return paramOne != paramTwo;
        break;
    }
    return false;
}

// Выполнить арфиметическую операцию
int varOpVar(int paramOne, int paramTwo, int operation) {
    switch(operation) {
    case 0:
        return paramTwo;
    case 1:
        return paramOne + paramTwo;
    case 2:
        return paramOne - paramTwo;
    case 3:
        return paramOne * paramTwo;
    case 4:
        return paramOne / paramTwo;
    case 5:
        return paramOne % paramTwo;
    }
    return 0;
}

// Выполнить программу
void run(const std::vector<std::pair<int, std::vector<int>>> & bytecode, std::istream & istr, std::ostream & ostr) {
    std::unordered_map<int, int> arr, vars;
    bool f;
    char t;
    for (unsigned int i = 0; i < bytecode.size(); i++) {
        switch (bytecode[i].first) {                                            // Коды операторов можно посмотреть в функции compile
        case 0:
            vars[bytecode[i].second[0]] = 0;
            break;
        case 7:
            vars[bytecode[i].second[0]]++;
            break;
        case 14:
            vars[bytecode[i].second[0]]--;
            break;
        case 1: case 8: case 15: case 22: case 29: case 36:
            vars[bytecode[i].second[0]] = varOpVar(vars[bytecode[i].second[0]], vars[bytecode[i].second[1]], bytecode[i].first / 7);
            break;
        case 2: case 9: case 16: case 23: case 30: case 37:
            vars[bytecode[i].second[0]] = varOpVar(vars[bytecode[i].second[0]], bytecode[i].second[1], bytecode[i].first / 7);
            break;
        case 3: case 10: case 17: case 24: case 31: case 38:
            vars[bytecode[i].second[0]] = varOpVar(vars[bytecode[i].second[1]], vars[bytecode[i].second[2]], bytecode[i].first / 7);
            break;
        case 4: case 11: case 18: case 25: case 32: case 39:
            vars[bytecode[i].second[0]] = varOpVar(vars[bytecode[i].second[1]], bytecode[i].second[2], bytecode[i].first / 7);
            break;
        case 5: case 12: case 19: case 26: case 33: case 40:
            vars[bytecode[i].second[0]] = varOpVar(bytecode[i].second[1], vars[bytecode[i].second[2]], bytecode[i].first / 7);
            break;
        case 6: case 13: case 20: case 27: case 34: case 41:
            vars[bytecode[i].second[0]] = varOpVar(bytecode[i].second[1], bytecode[i].second[2], bytecode[i].first / 7);
            break;
        case 42:
            arr[vars[bytecode[i].second[0]]] = vars[bytecode[i].second[1]];
            break;
        case 43:
            arr[vars[bytecode[i].second[0]]] = bytecode[i].second[1];
            break;
        case 44:
            arr[bytecode[i].second[0]] = vars[bytecode[i].second[1]];
            break;
        case 45:
            arr[bytecode[i].second[0]] = bytecode[i].second[1];
            break;
        case 46:
            vars[bytecode[i].second[0]] = arr[vars[bytecode[i].second[1]]];
            break;
        case 47:
            vars[bytecode[i].second[0]] = arr[bytecode[i].second[1]];
            break;
        case 48:
            f = setFlag(vars[bytecode[i].second[0]], vars[bytecode[i].second[1]], bytecode[i].second[2]);
            if (!f) {
                i = bytecode[i].second[3] - 1;
            }
            break;
        case 49:
            f = setFlag(vars[bytecode[i].second[0]], bytecode[i].second[1], bytecode[i].second[2]);
            if (!f) {
                i = bytecode[i].second[3] - 1;
            }
            break;
        case 50:
            f = setFlag(bytecode[i].second[0], vars[bytecode[i].second[1]], bytecode[i].second[2]);
            if (!f) {
                i = bytecode[i].second[3] - 1;
            }
            break;
        case 51:
            f = setFlag(bytecode[i].second[0], bytecode[i].second[1], bytecode[i].second[2]);
            if (!f) {
                i = bytecode[i].second[3] - 1;
            }
            break;
        case 52:
            i = bytecode[i].second[0] - 1;
            break;
        case 54:
            istr >> t;
            vars[bytecode[i].second[0]] = t;
            break;
        case 55:
            t = vars[bytecode[i].second[0]];
            ostr << t;
            break;
        case 56:
            t = bytecode[i].second[0];
            ostr << t;
            break;
        case 57:
            istr >> vars[bytecode[i].second[0]];
            break;
        case 58:
            ostr << vars[bytecode[i].second[0]] << std::endl;
            break;
        case 59:
            ostr << bytecode[i].second[0] << std::endl;
            break;
        }
    }
}
