/* main.hpp */
// Основной заголовочный файл проекта

//Заголовочные файлы для работы с потоками ввода-вывода
#include <sstream>

//Заголовочные файлы для подключения возможностей STL
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>

#include "slang.hpp"

void delSpaces(std::string &);
std::string getWord(const std::string &);
void delWord(std::string &);
void error(int, int, std::string = "");
bool isNumber(std::string);
int getVar(std::string &, std::map<std::string, int>, int);
int getCom(std::string &, std::map<std::string, int>, int, bool &);
int getOp(std::string &, std::map<std::string, int>, int);
void getInit(std::string &, std::map<std::string, int> &, int);
int findBracket(const std::vector<int> &, const int &, const int &, int, bool &);
int ifDepthest(std::vector<int>, int);
int addConst(std::map<std::string, int> &, std::vector<std::pair<int, int>> &, int);
void compile(std::string, std::vector<std::pair<int, std::vector<int>>> &, std::vector<std::pair<int, int>> &);
bool setFlag(int, int, int);
int varOpVar(int, int, int);
void setConsts(int *, const std::vector<std::pair<int, int>> &);
int analisys(const std::vector<std::pair<int, std::vector<int>>> &);
void run(const std::vector<std::pair<int, std::vector<int>>> &, std::vector<std::pair<int, int>> &, std::istream &, std::ostream &);
