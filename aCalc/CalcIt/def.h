#ifndef DEF_H
#define DEF_H

class Token;
class Variable;


#define _QT4
//#define _UNIX
//#define _QT_INTERFACE


#ifdef _QT4
    #include <QString>
    #include <QList>
    #include <QMap>
    typedef QString String;
    typedef QChar Char;
    typedef QList<Token> TokenList;
    typedef QMap<String, Variable> VarList;
    typedef QMap<String, String> MapStrings;
    typedef QMap<QChar, String> MapCharString;

    #ifdef HAVE_QT5
        #define toAscii toStdString().c_str
    #endif
#else
    #include <string>
    #include <list>
    #include <map>
    #include <cstdlib>
    typedef std::string String;
    typedef char Char;
    typedef std::list<Token> TokenList;
    typedef std::map<String, Variable> VarList;
    typedef std::map<String, String> MapStrings;
    typedef std::map<char, String> MapCharString;
    #define toAscii c_str
#endif

typedef unsigned int UINT;


#endif // DEF_H
