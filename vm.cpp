/* vm.cpp */

/* Язык программирования slang, виртуальная машина
 * Лицензия: GNU GPL v3
 * Автор: Кручинин Тимофей <tymcrit.gmail.com>
 * Данное програмное обеспечение распространяется КАК ЕСТЬ и не предоставляет никаких гарантий
 */

#include "main.hpp"

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

void setConsts(int * vars, const std::vector<std::pair<int, int>> & consts) {
    for (unsigned int i = 0; i < consts.size(); i++) {
        vars[consts[i].first] = consts[i].second;
    }
}

int analisys(const std::vector<std::pair<int, std::vector<int>>> & bytecode) {
    int ret = 0;
    for (unsigned int i = 0; i < bytecode.size(); i++) {
        switch (bytecode[i].first) {
        case 0: case 7: case 8: case 9:
            ret = std::max(ret, std::max(bytecode[i].second[0], bytecode[i].second[1]));
            break;
        case 12: case 13: case 14: case 15:
            ret = std::max(ret, bytecode[i].second[0]);
            break;
        case 2: case 3: case 4: case 5: case 6:
            ret = std::max(ret, std::max(bytecode[i].second[0], std::max(bytecode[i].second[1], bytecode[i].second[2])));
            break;
       }
    }
    return ret;
}

// Выполнить программу
void run(const std::vector<std::pair<int, std::vector<int>>> & bytecode, std::vector<std::pair<int, int>> & consts, std::istream & istr, std::ostream & ostr) {
    std::unordered_map<int, int> arr;
    int vars[analisys(bytecode)];
    setConsts(vars, consts);
    consts.clear();
    bool f;
    char t;
    for (unsigned int i = 0; i < bytecode.size(); i++) {
        switch (bytecode[i].first) {                                            // Коды операторов можно посмотреть в функции compile
        case 0: 
            vars[bytecode[i].second[0]] = vars[bytecode[i].second[1]];
            break;
        case 2: case 3: case 4: case 5: case 6:
            vars[bytecode[i].second[0]] = varOpVar(vars[bytecode[i].second[1]], vars[bytecode[i].second[2]], bytecode[i].first - 1);
            break;
        case 7:
            arr[vars[bytecode[i].second[0]]] = vars[bytecode[i].second[1]];
            break;
        case 8:
            vars[bytecode[i].second[0]] = arr[vars[bytecode[i].second[1]]];
            break;
        case 9:
            f = setFlag(vars[bytecode[i].second[0]], vars[bytecode[i].second[1]], bytecode[i].second[2]);
            if (!f) {
                i = bytecode[i].second[3] - 1;
            }
            break;
        case 10:
            i = bytecode[i].second[0] - 1;
            break;
        case 12:
            istr >> t;
            vars[bytecode[i].second[0]] = t;
            break;
        case 13:
            t = vars[bytecode[i].second[0]];
            ostr << t;
            break;
        case 14:
            istr >> vars[bytecode[i].second[0]];
            break;
        case 15:
            ostr << vars[bytecode[i].second[0]] << std::endl;
            break;
        }
    }
}
