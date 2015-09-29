#ifndef TOKEN_H
#define TOKEN_H

#include "def.h"
#include "consts.h"

class Token
{
public:
    Token();
    Token(String Val, int token_type, bool prefix = false);
    const String& Value();
    void Clear();
    void SetType(int token_type);
    void SetErr(int err);
    void SetValue(String s);
    void Add(Char ch);
    void Add(String s);
    bool CutNumber();
    int Type();
    int Err();
    //Token& operator=(const Token& t);

    e_type_var type_var;
    bool prefix;
private:
    int token_type;
    String value;
    int err;
};

#endif // TOKEN_H
