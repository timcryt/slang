//Заголовочные файлы для работы с потоками ввода-вывода
#include <sstream>

//Заголовочные файлы для подключения возможностей STL
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>

#include "slang.hpp"

void delSpaces(std::string & s);
std::string getWord(const std::string & s);
void delWord(std::string & s);
void error(int line, int code, std::string msg = "");
bool isNumber(std::string s);
int getVar(std::string & s, std::map<std::string, int> v, int line);
int getCom(std::string & s, std::map<std::string, int> v, int line, bool & f);
int getOp(std::string & s, std::map<std::string, int> v, int line);
void getInit(std::string & s, std::map<std::string, int> & vars, int line);
int findBracket(const std::vector<int> & brackets, const int & ifDepth, const int & whileDepth, int index, bool & flag);
int ifDepthest(std::vector<int> brackets, int index);
void compile(std::string s, std::vector<std::pair<int, std::vector<int>>> & comp);
bool setFlag(int paramOne, int paramTwo, int compare);
void varOpVar(int & paramOne, int paramTwo, int operation);
void run(const std::vector<std::pair<int, std::vector<int>>> & bytecode, int kolvar, std::istream & istr, std::ostream & ostr);
int analisys(std::vector<std::pair<int, std::vector<int>>> code) ;
