/* Язык программирования slang, версия 1.0 от 29.12.18
 * Лицензия: GNU GPL v3
 * Автор: Кручинин Тимофей <tymcrit.gmail.com>
 * Данное програмное обеспечение распространяется КАК ЕСТЬ и не предоставляет никаких гарантий
 */


//Заголовочные файлы для работы с потоками ввода-вывода
#include <iostream>
#include <fstream>
#include <sstream>

//Заголовочные файлы для подключения возможностей STL
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>

using namespace std;

// Функция delspaces приводит строку в приличный вид - заменяет все табы пробелами, удаляет все двойные пробелы, удаляет все пробелы из начала строки и добавляет один пробел в конец, если его там не было
void delSpaces(string & s) {
    while (s.find("\t") != string().npos) {
        s.replace(s.begin() + s.find("\t"), s.begin() + s.find("\t") + 1, " ");
    }
    while (s.find("  ") != string().npos) {
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
string getWord(const string & s) {
      return s.substr(0, s.find(" "));
}

// Удалить первое слово из приведённой в порядок строки
void delWord(string & s) {
    s.erase(s.begin(), s.begin() + s.find(" ") + 1);
}

// Выдать ощибку и аварийно завершиться
void error(int line, int code, string msg = "") {
    string errArr[] = {"", "unknown command", "variable can't be a number", "unknown variable", "too few arguments", "to more arguments", "expected variable, found", "expected comparsion operator, found", "unbracket end", "else without if"};
    ostringstream err;
    err << "Line " << line << ": error: " << errArr[code] << " " << msg << endl;
    throw err.str();
}


// Проверка строки на число
bool isNumber(string s) {
    return string("0123456789-").find(s.at(0)) != string().npos && s.substr(1).find_first_not_of("0123456789") == string().npos;
}

// ПОлучить код переменной из строки
int getVar(string & s, map<string, int> v, int line) {
    if (s.empty()) {
        error(line, 4);
    }
    string w = getWord(s);
    delWord(s);
    if (v.find(w) == v.end()) {
        error(line, 3, w);
    }
    return v[w];
}

// Получить код переменную или число из строки
int getCom(string & s, map<string, int> v, int line, bool & f) {
    if (s.empty()) {
        error(line, 4);
    }
    string w = getWord(s);
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
int getOp(string & s, map<string, int> v, int line) {
    const map<string, int> mp = {
        pair<string, int>("<", 0),
        pair<string, int>("<=", 1),
        pair<string, int>("=", 2),
        pair<string, int>(">=", 3),
        pair<string, int>(">", 4),
        pair<string, int>("<>", 5)
    };
    if (s.empty()) {
        error(line, 4);
    }
    string w = getWord(s);
    delWord(s);
    if (mp.find(w) == mp.end()) {
        error(line, 7, w);
    }
    return mp.at(w);
}

// Получить переменную для инициализации
void getInit(string & s, map<string, int> & vars, int line) {
    string var = getWord(s);
    delWord(s);
    if (isNumber(var)) {
        error(line, 2);
    }
    vars[var] = vars.size();
}

// Найти парный блочный оператор
int findBracket(const vector<int> & brackets, const int & ifDepth, const int & whileDepth, int index, bool & flag) {
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
int ifDepthest(vector<int> brackets, int index) {
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
void compile(string s, vector<pair<int, vector<int>>> & comp) {
    static int line, ifDepth, whileDepth;
    const int commandIndex = comp.size();
    static map<string, int> vars;
    static vector<int> jumps = {0};
    line++;
    jumps.push_back(0);
    delSpaces(s);
    if (s.empty()) {
        return;
    }
    string command = getWord(s);
    delWord(s);
    const map<string, int> commands = {                                         // Список всех команд
        pair<string, int>("var", -1),
        pair<string, int>("#", -1),
        pair<string, int>("set", 0),
        pair<string, int>("+", 2),
        pair<string, int>("-", 4),
        pair<string, int>("*", 6),
        pair<string, int>("/", 8),
        pair<string, int>("%", 10),
        pair<string, int>("arrset", 12),
        pair<string, int>("arrget", 16),
        pair<string, int>("while", 18),
        pair<string, int>("if", 18),
        pair<string, int>("end", 26),
        pair<string, int>("else", 26),
        pair<string, int>("in", 28),
        pair<string, int>("out", 30),
        pair<string, int>("read", 32),
        pair<string, int>("print", 34)
    };
    const set<string> varcom = {"set", "+", "-", "*", "/", "%", "arrget"};      // Список команд, параметрами которых являются переменная и значение
    const set<string> comcom = {"arrset"};                                      // Список команд, параметрами которых являются два значения
    const set<string> block = {"while", "if"};                                  // Список блочных команд
    const set<string> com = {"in", "out", "read", "print"};                     // Список команд, параметром которых является значение
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
        comp.push_back(pair<int, vector<int>>(commands.at(command) + (varFlag ? 0 : 1), vector<int>{var, arg}));
    } else if (comcom.find(command) != comcom.end()) {
        bool indexFlag, elementFlag;
        int index = getCom(s, vars, line, indexFlag);
        int element = getCom(s, vars, line, elementFlag);
        comp.push_back(pair<int, vector<int>>(commands.at(command) + (elementFlag ? 0 : 1) + (indexFlag ? 0 : 2), vector<int>{index, element}));
    } else if (block.find(command) != block.end()) {
        bool paramOneFlag, paramTwoFlag;
        int paramOne = getCom(s, vars, line, paramOneFlag);
        int comparsion = getOp(s, vars, line);
        int paraTwo = getCom(s, vars, line, paramTwoFlag);
        comp.push_back(pair<int, vector<int>>(commands.at(command) + (paramOneFlag ? 0 : 2) + (paramTwoFlag ? 0 : 1), vector<int>{paramOne, paraTwo, comparsion, -1}));
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
            comp.push_back(pair<int, vector<int>>(commands.at(command), vector<int>{commandIndex + 1}));
            jumps[commandIndex] = 3;
            ifDepth--;
        } else {
            comp.push_back(pair<int, vector<int>>(commands.at(command), vector<int>{index}));
            jumps[commandIndex] = 4;
            whileDepth--;
        }
    } else if (com.find(command) != com.end()) {
        bool paramFlag;
        int param = getCom(s, vars, line, paramFlag);
        comp.push_back(pair<int, vector<int>>(commands.at(command) + (paramFlag ? 0 : 1), vector<int>{param}));
    } else if (command == "else") {
        int index;
        if ((index = ifDepthest(jumps, commandIndex)) == -1) {
            error(line, 11);
        }
        comp[index].second[3] = commandIndex + 1;
        comp.push_back(pair<int, vector<int>>(commands.at(command), vector<int>{-1}));
        jumps[commandIndex] = 5;
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
void varOpVar(int & paramOne, int paramTwo, int operation) {
    switch(operation) {
    case 0:
        paramOne = paramTwo;
        break;
    case 1:
        paramOne += paramTwo;
        break;
    case 2:
        paramOne -= paramTwo;
        break;
    case 3:
        paramOne *= paramTwo;
        break;
    case 4:
        paramOne /= paramTwo;
        break;
    case 5:
        paramOne %= paramTwo;
        break;
    }
}

// Выполнить программу
void run(const vector<pair<int, vector<int>>> & bytecode, int kolvar) {
    int vars[kolvar];
    unordered_map<int, int> arr;
    bool f;
    char t;
    for (unsigned int i = 0; i < bytecode.size(); i++) {
        switch (bytecode[i].first) {                                            // Коды операторов можно посмотреть в функции compile
        case 0:
        case 2:
        case 4:
        case 6:
        case 8:
        case 10:
            varOpVar(vars[bytecode[i].second[0]], vars[bytecode[i].second[1]], bytecode[i].first / 2);
            break;
        case 1:
        case 3:
        case 5:
        case 7:
        case 9:
        case 11:
            varOpVar(vars[bytecode[i].second[0]], bytecode[i].second[1], bytecode[i].first / 2);
            break;
        case 12:
            arr[vars[bytecode[i].second[0]]] = vars[bytecode[i].second[1]];
            break;
        case 13:
            arr[vars[bytecode[i].second[0]]] = bytecode[i].second[1];
            break;
        case 14:
            arr[bytecode[i].second[0]] = vars[bytecode[i].second[1]];
            break;
        case 15:
            arr[bytecode[i].second[0]] = bytecode[i].second[1];
            break;
        case 16:
            vars[bytecode[i].second[0]] = arr[vars[bytecode[i].second[1]]];
            break;
        case 17:
            vars[bytecode[i].second[0]] = arr[bytecode[i].second[1]];
            break;
        case 18:
            f = setFlag(vars[bytecode[i].second[0]], vars[bytecode[i].second[1]], bytecode[i].second[2]);
            if (!f) {
                i = bytecode[i].second[3] - 1;
            }
            break;
        case 19:
            f = setFlag(vars[bytecode[i].second[0]], bytecode[i].second[1], bytecode[i].second[2]);
            if (!f) {
                i = bytecode[i].second[3] - 1;
            }
            break;
        case 20:
            f = setFlag(bytecode[i].second[0], vars[bytecode[i].second[1]], bytecode[i].second[2]);
            if (!f) {
                i = bytecode[i].second[3] - 1;
            }
            break;
        case 21:
            f = setFlag(bytecode[i].second[0], bytecode[i].second[1], bytecode[i].second[2]);
            if (!f) {
                i = bytecode[i].second[3] - 1;
            }
            break;
        case 26:
            i = bytecode[i].second[0] - 1;
            break;
        case 28:
            cin >> t;
            vars[bytecode[i].second[0]] = t;
            break;
        case 30:
            t = vars[bytecode[i].second[0]];
            cout << t;
            break;
        case 31:
            t = bytecode[i].second[0];
            cout << t;
            break;
        case 32:
            cin >> vars[bytecode[i].second[0]];
            break;
        case 34:
            cout << vars[bytecode[i].second[0]] << endl;
            break;
        case 35:
            cout << bytecode[i].second[0] << endl;
            break;
        }
    }
}

// Провести посткомпиляционный анализ кода
int analisys(vector<pair<int, vector<int>>> code) {
    int m = 0;
    for(unsigned int i = 0; i < code.size(); i++) {
        switch (code[i].first) {
        case 0:
        case 2:
        case 4:
        case 6:
        case 8:
        case 10:
        case 12:
        case 16:
        case 18:
            m = max(m, max(code[i].second[0], code[i].second[1]));
            break;
        case 1:
        case 3:
        case 5:
        case 7:
        case 9:
        case 11:
        case 17:
        case 19:
        case 28:
        case 30:
        case 31:
        case 32:
        case 34:
        case 35:
            m = max(m, code[i].second[0]);
            break;
        case 14:
        case 20:
            m = max(m, code[i].second[1]);
            break;
        }
    }
    return m;
}

int main(int argc, char ** argv) {
    if (argc != 2) {
        cerr << "Error: argument error" << endl;
        cerr << "Using: slang <file>" << endl;
        exit(1);
    }
    ifstream file;
    file.open(argv[1]);
    if (!file.is_open()) {
        cerr << "Error: can't open file" << endl;
        exit(1);
    }
    vector<pair<int, vector<int>>> compiled;
    while (!file.eof()) {
        string s;
        getline(file, s);
        try {
            compile(s, compiled);
        } catch (string message) {
            cerr << message;
            return 1;
        }
    }
    int kolvar = analisys(compiled);
    run(compiled, kolvar);
    return 0;
}
