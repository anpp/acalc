#ifndef TOKEN_H
#define TOKEN_H

#include "def.h"
#include "consts.h"

class Token
{
public:
    Token();
    Token(String Val, t_type token_type, bool prefix = false);
    const String& Value();
    void Clear();
    void SetType(t_type token_type);
    void SetErr(errors err);
    void SetValue(String s);
    void Add(Char ch);
    void Add(String s);
    bool CutNumber();
    t_type Type();
    errors Err();
    //Token& operator=(const Token& t);

    e_type_var type_var;
    bool prefix;
private:
    t_type token_type;
    String value;
    errors err;
};

#endif // TOKEN_H
