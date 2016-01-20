#ifndef DEF_H
#define DEF_H

class Token;
class Variable;


#include <QString>
#include <string>
#include <list>
#include <map>
#include <cstdlib>


typedef QString String;
typedef QChar Char;
typedef std::list<Token> TokenList;
typedef std::map<String, Variable> VarList;
typedef std::map<String, String> MapStrings;

typedef unsigned int UINT;


#endif // DEF_H
