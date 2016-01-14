#include "token.h"

Token::Token()
{
    Clear();
}

Token::Token(String Val, t_type token_type, bool prefix)
{
    Clear();
    this->SetValue(Val);
    this->SetType(token_type);
    this->prefix = prefix;
}

const String& Token::Value()
{
    return value;
}


void Token::Clear()
{
    token_type = t_type::NOPE;
    value = "";
    err = errors::SUCCESS;
    type_var = FLOAT;
    prefix = false;
}

void Token::SetType(t_type token_type)
{
    this->token_type = token_type;
}

void Token::SetErr(errors err)
{
    this->err = err;
}


void Token::SetValue(String s)
{
    value = s;
}

void Token::Add(Char ch)
{
    value += ch;
}

void Token::Add(String s)
{
    value += s;
}


bool Token::CutNumber()
{
    if(token_type != t_type::NUMBER)
        return false;
    if(value.isEmpty())
        return false;
    value.resize(value.size() - 1);
    return !value.isEmpty();
}


t_type Token::Type()
{
   return token_type;
}

errors Token::Err()
{
   return err;
}


/*
Token& Token::operator=(const Token& t)
{
    Token *result = new Token();

    result->token_type = t.token_type;
    result->value = t.value;
    return *result;
}

*/
