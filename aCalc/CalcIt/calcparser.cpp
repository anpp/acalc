#include "calcparser.h"
#include <QObject>

#define tr(text) QObject::tr(text)


String _Errors[] = {tr("Unknown error!"), tr("Syntax error"), tr("Unclosed parenthesis")
    ,tr("Unknown function"), tr("Division by zero!"),
                  tr("Invalid argument"), tr("Value is too large"), tr("Undefined variable"),
                  tr("Empty parentheses"), tr("Type mismatch"), tr("Readonly variable")};



String VarTypes[] = {"float", "string"};




//----------------------------------------------------------------------------------------------------------------------
CalcParser::CalcParser(String *pexpr): PI(acos(-1))
{
    result = new CValue(FLOAT);

    dec_point = "";
    eq = "";
    token_end.SetType(t_type::END);
    token_end.SetValue("END");
    SetParams(pexpr);
    InitMapToksHtml();
    InitFuncs();

    SetVariable("pi", DoubleToString(PI), FLOAT, true);
    SetVariable("e", DoubleToString(exp(1)), FLOAT, true);
}

//----------------------------------------------------------------------------------------------------------------------
CalcParser::~CalcParser()
{
    Tokens.clear();
    delete result;
}


//----------------------------------------------------------------------------------------------------------------------
void CalcParser::SetVariable(String name, String value, e_type_var type, bool readonly)
{
    if(name.isEmpty() || name == "") return;

    Variable var(name, value, type);
    var.read_only = readonly;
    Vars[var.name] = var;
}


//----------------------------------------------------------------------------------------------------------------------
bool CalcParser::Run(void)
{
    bool res = true;
    result->SetValue(0);
    i_toks = Tokens.begin();
    do
    {
        res &= Calculate(result);
    }while(token->Type() == t_type::ENDLINE);
    return res;
}

//----------------------------------------------------------------------------------------------------------------------
bool CalcParser::Calculate(CValue *loc_result)
{
    GetToken();
    while(token->Type() != t_type::ENDLINE && token->Type() != t_type::END)
    {
        result->SetType(FLOAT);
        Add_exp(loc_result);
        if(token->Type() != t_type::ENDLINE && token->Type() != t_type::END)
            Error(errors::SYNTAX);

    }
    return !HasErr();
}

//----------------------------------------------------------------------------------------------------------------------
bool CalcParser::InitVariableFromExpression()
{
    Token t0, t1;
    Token *t = token;
    String var;
    TokenList::iterator it = i_toks;
    CValue res;
    Variable v;

    t0 = *token;
    if(t0.Type() == t_type::VARIABLE)
    {
        GetToken();
        t1 = *token;
        if(t1.Value() == "=")
        {
            var = t0.Value();

            binit_var = true;

            GetToken();
            Add_exp(&res);
            if(!this->HasErr())
            {
                VarList::const_iterator var_it = Vars.find(var);
                if(var_it != Vars.end())
                {
                    v = VariableByIterator(var_it);
                    if(v.read_only)
                    {
                        Error(errors::RO);
                        return false;
                    }

                }

                v.type = t0.type_var;
                if(v.type != res.Type())
                {
                    Error(errors::TMS);
                    return false;
                }
                v.name = var;
                if(v.type == FLOAT)
                    v.value = DoubleToString(res.ValueFloat());
                else
                    v.value = res.ValueString();

                Vars[var] = v;
                return true;
            }
            else
                return false;

        }
    }
    i_toks = it;
    token = t;
    return false;
}

//----------------------------------------------------------------------------------------------------------------------
void CalcParser::InitExpr(String *pexpr)
{
    binit_var = false;
    if(pexpr == nullptr)
        expr = "";
    else
        expr = *pexpr;

    lc = localeconv();// надо перенести в расчет
    if(lc)
        dec_point.insert(0, lc->decimal_point);

    token = &token_end;
}


//----------------------------------------------------------------------------------------------------------------------
void CalcParser::SetParams(String *pexpr, int scale, int DRG_mode)
{
    err = errors::SUCCESS;
    result->SetType(FLOAT);

    InitExpr(pexpr);

    SetScale(scale);
    SetDRG(DRG_mode);
    LoadTokens();
}


//----------------------------------------------------------------------------------------------------------------------
void CalcParser::SetScale(int scale)
{
    Token *t;
    long double number;
    String s;
    int prev_scale = this->scale;

    this->scale = scale;

    for(auto i = Tokens.begin(); i != Tokens.end(); ++i)
    {
        t = &(*i);
        if(t->Type() == t_type::NUMBER)
        {
            number = ScaleToVal(t->Value(), prev_scale);
            t->SetValue(DoubleToString(number));
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------
int CalcParser::Scale()
{
    return scale;
}


//----------------------------------------------------------------------------------------------------------------------
void CalcParser::SetDRG(int drg_mode)
{
    if(drg_mode == 0)
        return;
    this->DRG_mode = drg_mode;
}


//----------------------------------------------------------------------------------------------------------------------
int CalcParser::DRG()
{
    return DRG_mode;
}


//----------------------------------------------------------------------------------------------------------------------
void CalcParser::LoadTokens()
{
    Token *nTok;

    exp_p = expr.begin();
    Tokens.clear();

    while((nTok = LoadToken()) != nullptr)
    {
        if(nTok->Type() == t_type::VARIABLE)
        {
            auto var_it = Vars.find(nTok->Value());
            if(var_it != Vars.end())
            {
                Variable v = VariableByIterator(var_it);
                nTok->type_var = v.type;
            }
        }
        if(nTok->Value() == ",")
            nTok->SetType(t_type::COMMA);
        Tokens.push_back(*nTok);
        delete nTok;
    }
}


//----------------------------------------------------------------------------------------------------------------------
int CalcParser::CheckParentheses()
{
   int res = 0;
   auto i = Tokens.begin();
   Token t;

   while(i != Tokens.end())
   {
       t = *i++;
       if(t.Value() == "(" || t.Type() == t_type::FUNCTION)
           res++;
       if(t.Value() == ")")
           res--;
   }
   return res;
}


//----------------------------------------------------------------------------------------------------------------------
TokenList::iterator CalcParser::FindParentheses()
{
    int res = 0;
    auto i = Tokens.end();
    Token t;

    if(Tokens.empty())
        return Tokens.end();

    while(i-- != Tokens.begin())
    {
        t = *i;

        if(t.Value() == "(" || t.Type() == t_type::FUNCTION)
            res++;
        if(t.Value() == ")")
            res--;
        if(0 == res)
            break;
    }
    if((t.Value() == "(" || t.Type() == t_type::FUNCTION) && res == 0)
        return i;
    return Tokens.end();
}


//----------------------------------------------------------------------------------------------------------------------
bool CalcParser::InvExpInNumber(Token* tok)
{
    int index;
    String value;

    if(!tok)
        return false;
    value = tok->Value();
    if(value.indexOf('e') == -1)
        return false;

    index = value.indexOf('+');
    if(index != -1)
    {
        value[index] = '-';
        tok->SetValue(value);
        return true;
    }
    index = value.indexOf('-');
    if(index != -1)
    {
        value[index] = '+';
        tok->SetValue(value);
        return true;
    }
    return false;
}


//----------------------------------------------------------------------------------------------------------------------
bool CalcParser::AddPrefixInverse()
{
    EraseErrors();

    Token *ltok = LastToken();
    Token before_tok;
    Token before_before_tok;
    auto i = Tokens.end();
    decltype(i) before_i;

        if(ltok->Type() == t_type::NUMBER || ltok->Value() == ")")
        {
            if(ltok->Type() == t_type::NUMBER)
                before_i = --i;
            else
                before_i = i = FindParentheses();

            if(i == Tokens.begin())
            {
                before_tok = *i;
                before_before_tok = token_end;
            }
            else
            {
                before_tok = *--i;
                if(i == Tokens.begin())
                    before_before_tok = *i;
                else
                    before_before_tok = *--i;
            }

            if(before_tok.Value() == "/" && before_before_tok.Value() == "1")
            {
                i = Tokens.erase(i);
                Tokens.erase(i);
            }
            else
            {
                before_i = Tokens.insert(before_i, *(new Token("/", t_type::DELIMITER)));
                Tokens.insert(before_i, *(new Token("1", t_type::NUMBER)));
            }
            return true;
        }
        return false;
}


//----------------------------------------------------------------------------------------------------------------------
bool CalcParser::AddPrefixOp(String op)
{
    EraseErrors();

    Token *ltok = LastToken();
    Token before_tok;
    auto i = Tokens.end();
    decltype(i) before_i;

    if(op == "1/")
        return AddPrefixInverse();

    if(op == "-" || op == "~")
    {
        if(ltok->Type() == t_type::NUMBER || ltok->Value() == ")")
        {
            if(ltok->Type() == t_type::NUMBER)
            {
                if(InvExpInNumber(ltok))
                    return true;
                else
                    before_i = --i;
            }
            else
                before_i = i = FindParentheses();

            if(i == Tokens.begin())
                before_tok = *i;
            else
                before_tok = *(--i);

            if((before_tok.Value() == op || before_tok.Value() == "+") && before_tok.prefix)
            {
                Tokens.erase(i);
                return true;
            }
            else
            {
                if(before_tok.Value() == "-" && before_tok.prefix && op == "~")
                    return false;
                if(before_tok.Value() == "~" && before_tok.prefix && op == "-")
                    return false;

                Tokens.insert(before_i, *(new Token(op, t_type::DELIMITER, true)));
                return true;
            }
        }
    }
    return false;
}


//----------------------------------------------------------------------------------------------------------------------
bool CalcParser::AddToken(String *pexpr)
{
    if(*pexpr == "+/-")
        return AddPrefixOp("-");
    if(*pexpr == "1/x")
        return AddPrefixOp("1/");
    if(*pexpr == "exp")
        *pexpr = ".e+";

    InitExpr(pexpr);
    Token *nTok;
    Token *ltok;
    bool res = true;

    EraseErrors();
    exp_p = expr.begin();

    while((nTok = LoadToken()) != nullptr)
    {
        if(nTok->Value() == "del") //для отладки
        {
            ToBack();
        }
        else
        {
            ltok = LastToken();

            if(nTok->Type() == t_type::NUMBER)
            {
                if(nTok->Value() == "." && (ltok->Type() == t_type::END || ltok->Type() != t_type::NUMBER))
                    nTok->SetValue("0" + nTok->Value());

                if(ltok->Type() == t_type::NUMBER)
                {
                    if(ltok->Value().indexOf('.') != -1 && nTok->Value() == ".e+")
                        nTok->SetValue("e+");

                    if(ltok->Value().indexOf('.') != -1 && nTok->Value().indexOf('.') != -1)
                        return false;
                    if(ltok->Value().indexOf('e') != -1 && nTok->Value().indexOf('e') != -1)
                        return false;
                    if(ltok->Value().indexOf('i') != -1 && nTok->Value().indexOf('i') != -1)
                        return false;

                    ltok->Add(nTok->Value());
                }
                else
                {
                    if(!(ltok->Value() == ")" || ltok->Type() == t_type::VARIABLE))
                        Tokens.push_back(*nTok);
                    else
                        res = false;
                }
            }
            else
                if(nTok->Value() == "(")
                {
                    if(ltok->Type() != t_type::NUMBER && ltok->Type() != t_type::VARIABLE && ltok->Value() != ")")
                        Tokens.push_back(*nTok);
                    else
                        res = false;
                }
            else
                if(nTok->Value() == ")")
                {
                    if(ltok->Type() == t_type::NUMBER || ltok->Type() == t_type::VARIABLE || ltok->Value() == ")")
                    {
                        Tokens.push_back(*nTok);
                        if(CheckParentheses() < 0)
                        {
                            nTok->SetValue("(");
                            Tokens.push_front(*nTok);
                        }
                    }
                    else
                        res = false;
                }
            else
                if(nTok->Type() == t_type::FUNCTION || nTok->Type() == t_type::VARIABLE)
                {
                    if(ltok->Type() != t_type::NUMBER && ltok->Value() != ")" && ltok->Type() != t_type::VARIABLE)
                        Tokens.push_back(*nTok);
                    else
                        res = false;
                }
            else
                if(nTok->Type() == t_type::DELIMITER)
                {
                    if((ltok->Type() != t_type::DELIMITER && ltok->Type() != t_type::END && ltok->Type() != t_type::FUNCTION)
                            || ltok->Value() == ")")
                    {
                        if(nTok->Value() == "~")
                            res = AddPrefixOp(nTok->Value());
                        else
                            Tokens.push_back(*nTok);
                    }
                    else
                    {
                        if((nTok->Value() == "+" || nTok->Value() == "-" || nTok->Value() == "~")

                                && (!ltok->prefix))
                        {
                            nTok->prefix = true;
                            Tokens.push_back(*nTok);
                        }
                        else
                            res = false;
                    }
                }
            else
                    res = false;
        }
        delete nTok;
        if(!res)
            break;

    }
    return res;
}

//----------------------------------------------------------------------------------------------------------------------
Token* CalcParser::LastToken()
{
    if(Tokens.empty())
        return &token_end;
    auto i = Tokens.end();
    Token *t = &(*--i);
    return t;
}


//----------------------------------------------------------------------------------------------------------------------
String CalcParser::LastTokenValue()
{
    return LastToken()->Value();
}


//----------------------------------------------------------------------------------------------------------------------
void CalcParser::ToBack(bool lastdigit)
{
    bool cutnumber = false;

    EraseErrors();
    Token *lastT = LastToken();

    if(lastT->Type() != t_type::END)
    {
        if(lastdigit)
            cutnumber = lastT->CutNumber();
        if(!cutnumber)
        {
            if(lastT->Value() == ")")
            {
                auto itP = FindParentheses();
                if(itP == Tokens.begin() && ((Token*)&(*itP))->Type() != t_type::FUNCTION)
                    Tokens.pop_front();
            }

            Tokens.pop_back();
            //i_toks = Tokens.end() - 1;
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------
e_type_var CalcParser::TypeRes()
{
    return result->Type();
}


//----------------------------------------------------------------------------------------------------------------------
bool CalcParser::HasErr()
{
    for (Token t: Tokens) { err = t.Err(); if (err != errors::SUCCESS) break;}
    return err != errors::SUCCESS;
}


//----------------------------------------------------------------------------------------------------------------------
long double CalcParser::GetResult()
{
    return RoundS(result->ValueFloat(), PRECISION_FOR_DOUBLE);
}


//----------------------------------------------------------------------------------------------------------------------
String CalcParser::GetResultStr()
{
    return result->ValueString();
}


//----------------------------------------------------------------------------------------------------------------------
void CalcParser::ReadVariableToken(Token *loc_token)
{
    while(isalpha(*exp_p) || strchr("1234567890", *exp_p))
        loc_token->Add(*exp_p++);
    if(*exp_p == '(')
    {
        loc_token->SetType(t_type::FUNCTION);
        loc_token->Add(*exp_p++);
    }
    else
        loc_token->SetType(t_type::VARIABLE);

}


//----------------------------------------------------------------------------------------------------------------------
Token* CalcParser::LoadToken()
{
    Token *loc_token;

    while(Space(*exp_p))
        ++exp_p;

    if (exp_p >= expr.end())
        return nullptr;

    loc_token = new Token();

    if(strchr("*/+-()%^!&|~=", *exp_p))
    {
        loc_token->SetType(t_type::DELIMITER);
        loc_token->Add(*exp_p++);
        //if(loc_token->Value() == "=")
        //    exp_p = expr.end();
    }

    else
    if(strchr("<>", *exp_p))
    {
        loc_token->SetType(t_type::DELIMITER);
        if(*exp_p == '<' && *(exp_p + 1) == '<')
            loc_token->Add("<<");
        else
        if(*exp_p == '>' && *(exp_p + 1) == '>')
            loc_token->Add(">>");
        else
            Error(errors::SYNTAX, loc_token);

        exp_p += 2;
    }
    else
        if(isalpha(*exp_p))
        {
            ReadVariableToken(loc_token);
            if(loc_token->Type() == t_type::VARIABLE)
            {
                if(loc_token->Value() == VarTypes[1]) //строковая переменная
                {
                    loc_token->SetValue("");
                    while(Space(*exp_p) && exp_p != expr.end())
                        ++exp_p;

                    if(isalpha(*exp_p))
                    {
                        ReadVariableToken(loc_token);
                        if(loc_token->Type() == t_type::VARIABLE)
                            loc_token->type_var = STRING;
                        else
                            Error(errors::SYNTAX, loc_token);
                    }
                    else
                        Error(errors::SYNTAX, loc_token);
                }
            }
        }
    else
    if(isdigit(*exp_p) && !strchr("ie+-", *exp_p))
    {
        while(isdigit(*exp_p))
        {
            if(*exp_p == 'e')
            {
                if(strchr("-+",*(exp_p + 1)))
                    loc_token->Add(*exp_p++);
                else
                    Error(errors::SYNTAX, loc_token);
            }
            loc_token->Add(*exp_p++);
        }
        loc_token->SetType(t_type::NUMBER);
    }
    else
    if(*exp_p == ';')
    {
        loc_token->Add(*exp_p++);
        loc_token->SetType(t_type::ENDLINE);
    }
    else
    if(*exp_p == '"')
    {
        ++exp_p;
        while(exp_p != expr.end() && *exp_p != '"')
            loc_token->Add(*exp_p++);

        if(*exp_p++ != '"')
            Error(errors::SYNTAX, loc_token);

        loc_token->SetType(t_type::TEXT);
        loc_token->type_var = STRING;
    }
    else
    {
        loc_token->Add(*exp_p++);
        loc_token->SetType(t_type::UNK);
    }
    return loc_token;
}


//----------------------------------------------------------------------------------------------------------------------
void CalcParser::GetToken()
{
    token = &token_end;
    if(i_toks == Tokens.end())
        return;
    token = &(*i_toks++);
}


//----------------------------------------------------------------------------------------------------------------------
String CalcParser::GetExpression(String eq, bool html)
{
    if(binit_var)
        return "";

    String es = "";
    String number_color, function_color, delimiter_color, default_color;
    String b_tag, e_tag;
    String def_expr = "0";
    Token t, next_t;
    bool empty = Tokens.empty();
    String space;
    String tokVal;
    MapStrings::const_iterator mit;

    if(empty)
        AddToken(&def_expr);

    auto tit = Tokens.begin();

    default_color = GetStrValueVariable("default_color");
    number_color = GetStrValueVariable("number_color");
    function_color = GetStrValueVariable("function_color");
    delimiter_color = GetStrValueVariable("delimiter_color");

    this->eq = eq;
    while(tit != Tokens.end())
    {
        t = *tit++;
        if(t.Type() != t_type::ERR)
        {
            tokVal = t.Value();

            b_tag = e_tag = "";

            space = " ";
            next_t = token_end;
            if(tit != Tokens.end())
            {
                next_t = *tit;
                if(next_t.Type() == t_type::ERR)
                    next_t.SetType(t_type::END);
            }

            if(html)
            {
                b_tag = "<FONT color=" + default_color + ">";
                if(t.Type() == t_type::NUMBER || t.prefix)
                    b_tag = "<FONT color=" + number_color + ">";
                if(t.Type() == t_type::FUNCTION)
                    b_tag = "<FONT color=" + function_color + ">";
                if(t.Type() == t_type::DELIMITER && t.Value() != ")" && t.Value() != "(" && !t.prefix)
                    b_tag = "<FONT color=" + delimiter_color + ">";
                e_tag = "</FONT>";

                if(t.Type() == t_type::FUNCTION)
                    tokVal.remove('(');

                mit = ToksHtml.find(tokVal);
                if(mit != ToksHtml.end())
                    tokVal = mit->second;
                if(t.Type() == t_type::FUNCTION)
                    tokVal += '(';
            }
                if(tokVal == "(" || next_t.Value() == ")" || t.Type() == t_type::FUNCTION || next_t.Type() == t_type::END || t.prefix)
                    space = "";

                es.append(b_tag + tokVal + e_tag + space);
        }
    }

    if(t.Value() != "=")
        es += eq;
    if(empty)
        Tokens.clear();
    return es;
}


//----------------------------------------------------------------------------------------------------------------------
bool CalcParser::Space(Char c)
{
    if(c == ' ' || c == ' ' || c == '\n') //пробел или таб или перевод строки
        return true;
    return false;
}


//----------------------------------------------------------------------------------------------------------------------
bool CalcParser::strchr(String t, Char c)
{
    if (t.indexOf(c) >= 0)
        return true;
    return false;
    }


//----------------------------------------------------------------------------------------------------------------------
void CalcParser::Error(errors c_err, Token *current_token)
{
    Token t;

    err = c_err;
    if(!current_token)
        current_token = token;

    if(current_token->Type() == t_type::END)
    {
        t.SetType(t_type::ERR);
        t.SetErr(err);
        t.SetValue("ERR");

        Tokens.push_back(t);
        i_toks = Tokens.end();
    }
    else
        current_token->SetErr(err);
}


//----------------------------------------------------------------------------------------------------------------------
const String CalcParser::listErrors()
{
    String errs = "";
    Token *t;

    for(auto it = Tokens.begin(); it != Tokens.end(); it++)
    {
        t = &(*it);
        if(t->Err() != errors::SUCCESS)
        {
            if(static_cast<unsigned int>(t->Err()) < sizeof(_Errors) / sizeof(String))
                errs.append(tr(_Errors[static_cast<int>(t->Err())].toStdString().c_str()));
            else
                errs.append("?");
            errs += "\n";
        }
    }
    return errs;
}


//----------------------------------------------------------------------------------------------------------------------
void CalcParser::EraseErrors()
{
   // if(err == -1)
   //     return;

    Token *t;

    for(auto it = Tokens.begin(); it != Tokens.end(); ++it)
    {
        t = &(*it);

        if(t->Type() == t_type::ERR)
        {
            it = Tokens.erase(it);
            --it;
            continue;
        }
        if(t->Err() != errors::SUCCESS)
            t->SetErr(errors::SUCCESS);
    }
    err = errors::SUCCESS;
}


//----------------------------------------------------------------------------------------------------------------------
bool CalcParser::isdigit(Char c)
{
    if((strchr("0123456789.ie", c) && scale == 10) || (((c >= 'A' && c <= 'F') || strchr("0123456789", c)) && scale == 16) ||
            (strchr("01", c) && scale == 2) || (strchr("01234567", c) && scale == 8) ||
            (((c >= 'A' && c <= 'F') || strchr("0123456789.e,", c)) && scale == 0))
        return true;
    return false;
}


//----------------------------------------------------------------------------------------------------------------------
bool CalcParser::isalpha(Char c)
{
    if((c >= 'a' && c <= 'z') || c == '_')
        return true;
    return false;
}


//----------------------------------------------------------------------------------------------------------------------
void CalcParser::Add_exp(CValue *res)
{
    CValue temp;
    Char op;

    Mul_exp(res);

    while(token->Value() == "+" || token->Value() == "-")
    {
        op = token->Value()[0];
        GetToken();
        Mul_exp(&temp);

        if(temp.Type() != res->Type())
            Error(errors::TMS);

        if(op == '+')
        {
            if(temp.Type() == STRING)
                res->SetValue(res->ValueString() + temp.ValueString());
            else
                res->SetValue(res->ValueFloat() + temp.ValueFloat());
        }
        if(op == '-')
            res->SetValue(res->ValueFloat() - temp.ValueFloat());
    }
}


//----------------------------------------------------------------------------------------------------------------------
void CalcParser::Mul_exp(CValue *res)
{
    CValue temp;
    Char op;
    char c_op;

    Step_exp(res);

    while(token->Value() == "*" || token->Value() == "/" || token->Value() == ">>" ||
          token->Value() == "<<" || token->Value() == "%" ||
        token->Value() == "&" || token->Value() == "|" || token->Value() == "!")
    {

        op = token->Value()[0];
    #ifdef HAVE_QT5
        c_op = (char)(op.toLatin1());
    #else
        c_op = (char)(op.toAscii());
    #endif

        GetToken();
        Step_exp(&temp);

        if(temp.Type() != res->Type())
        {
            Error(errors::TMS);
            break;
        }

        switch(c_op)
        {
        case '*':
            res->SetValue(RoundS((res->ValueFloat() * temp.ValueFloat()), PRECISION_FOR_DOUBLE));
            break;
        case '/':
            if(temp.ValueFloat() == 0)
                Error(errors::DIVISION);
            else
                res->SetValue(res->ValueFloat() / temp.ValueFloat());
            break;
        case '<':
            res->SetValue((UINT)res->ValueFloat() << (UINT)temp.ValueFloat());
            break;
        case '>':
            res->SetValue((UINT)res->ValueFloat() >> (UINT)temp.ValueFloat());
            break;
        case '%':
            if(temp.ValueFloat() == 0)
                Error(errors::DIVISION);
            res->SetValue(fmod(res->ValueFloat(), temp.ValueFloat()));
            break;
        case '&':
            res->SetValue((int)res->ValueFloat() & (int)temp.ValueFloat());
            break;
        case '|':
            res->SetValue((int)res->ValueFloat() | (int)temp.ValueFloat());
            break;
        case '!':
            res->SetValue((int)res->ValueFloat() ^ (int)temp.ValueFloat());
            break;
        }

    }
}


//----------------------------------------------------------------------------------------------------------------------
void CalcParser::Step_exp(CValue *res)
{
    CValue temp;

    Sign_exp(res);

    if((token->Type() == t_type::DELIMITER) && (token->Value() == "^"))
    {
        GetToken();
        Step_exp(&temp);

        if(temp.Type() != res->Type())
            Error(errors::TMS);

        res->SetValue(RoundS(pow(res->ValueFloat(), temp.ValueFloat()), PRECISION_FOR_DOUBLE));
    }
}



//----------------------------------------------------------------------------------------------------------------------
void CalcParser::Sign_exp(CValue *res)
{
    Char op = ' ';

    if((token->Type() == t_type::DELIMITER) &&
        (token->Value() == "+" || token->Value() == "-" || token->Value() == "~"))
    {
        op = token->Value()[0];
        GetToken();
    }
    Scob_exp(res);
    if(op == '-')
        res->SetValue(-res->ValueFloat());
    else
        if(op == '~')
            res->SetValue(~(int)res->ValueFloat());
}


//----------------------------------------------------------------------------------------------------------------------
void CalcParser::Scob_exp(CValue *res)
{
    CValue temp;

    if(token->Type() == t_type::DELIMITER && token->Value() == "(")
    {
        GetToken();
        if(token->Value() == ")")
            Error(errors::EMPTBKT);
        else
        {
            Add_exp(&temp);

            if(temp.Type() != res->Type())
                Error(errors::TMS);

            res->SetValue(temp.ValueFloat());
            if(token->Value() != ")")
                Error(errors::BKT);

        }
        GetToken();
        if(token->Type() == t_type::NUMBER)
            Error(errors::SYNTAX);

    }
    else
        GetNumber(res);
}



//----------------------------------------------------------------------------------------------------------------------
void CalcParser::GetNumber(CValue *res)
{
    CValue temp;
    CValue temp1;
    Function* f;
    unsigned f_n_par;
    String var;
    VarList::const_iterator var_it;
    Variable v;
    Token* ftok;

    switch(token->Type())
    {
    case t_type::NUMBER:
        {
        res->SetValue(ScaleToVal(token->Value(), scale));
        GetToken();
        break;
        }
    case t_type::FUNCTION:
        {
        ftok = token;
        auto fi = map_funcs.find(token->Value());
        if (fi != map_funcs.end())
        {
            f = fi->second;
            f_n_par = f->getCountAgrs();
            GetToken();
            Add_exp(&temp);
            switch(f_n_par)
            {
            case 1:
                if(token->Value() != ")")
                    Error(errors::BKT);
                res->SetValue(CheckNumberZero(f->Calculate(temp.ValueFloat(), DRG_mode, ftok)));
                break;
            case 2:
                if(token->Type() != t_type::COMMA)
                {
                    Error(errors::SYNTAX);
                    break;
                }
                GetToken();
                Add_exp(&temp1);
                if(token->Value() != ")")
                {
                    Error(errors::BKT);
                    break;
                }
                res->SetValue(f->Calculate(temp.ValueFloat(), temp1.ValueFloat(), ftok));
                break;
            default:
                Error(errors::UNKNOWN);
                }
            GetToken();
            break;
            }
        else
            Error(errors::UNDEFINED);
        }
    case t_type::VARIABLE:
        var = token->Value();
        if(!InitVariableFromExpression()) //если выражение содержит инициализацию переменных
            GetToken();
        var_it = Vars.find(var);
        if(var_it != Vars.end())
        {
            v = VariableByIterator(var_it);
            if(v.type == FLOAT)
                res->SetValue(ScaleToVal(v.value));
            else
            {
                res->SetType(v.type);
                res->SetValue(v.value);
            }

        }
        else
            Error(errors::UNKVAR);
        break;
    case t_type::TEXT:
        res->SetType(token->type_var);
        res->SetValue(token->Value());
        GetToken();
        break;
    default:
        Error(errors::SYNTAX);
        GetToken();
    }
}


//----------------------------------------------------------------------------------------------------------------------
String CalcParser::GetStrValueVariable(String varname)
{
    auto var_it = Vars.find(varname);
    Variable v;
    if(var_it != Vars.end())
    {
        v = VariableByIterator(var_it);
        return v.value;
    }
    return "";
}



//----------------------------------------------------------------------------------------------------------------------
long double CalcParser::ScaleToVal(String s, int scale)
{
    int len, i, j;
    long double res = 0;
    String tst = "";
    String::iterator val;
    if(!dec_point.isEmpty())
    {
        for(auto it = s.begin(); it != s.end(); ++it)
            if(*it == '.' && dec_point != ".")
                *it = dec_point[0];
    }

    if(10 == scale)
    {
        if(s.indexOf('e') != -1)
        {
            val = s.begin() + s.indexOf('e') + 1;
            for(; val != s.end(); val++)
                tst += *val;
            s.resize(s.indexOf('e'));
            res = atof(s.toStdString().c_str()) * pow(10, atoi(tst.toStdString().c_str()));

        }
        else
            res = atof(s.toStdString().c_str());
    }
    else
    {
        res = 0;
        len = s.size();
        j = 0;
        for(i = len - 1; i >= 0; i--)
        {
            tst = "";
            if(*(s.begin() + i) > '9')
            {
                if(*(s.begin() + i) == 'A')
                    tst += "10";
                if(*(s.begin() + i) == 'B')
                    tst += "11";
                if(*(s.begin() + i) == 'C')
                    tst += "12";
                if(*(s.begin() + i) == 'D')
                    tst += "13";
                if(*(s.begin() + i) == 'E')
                    tst += "14";
                if(*(s.begin() + i) == 'F')
                    tst += "15";
            }
            else
                tst += *(s.begin() + i);

            res += pow(scale, j) * atoi(tst.toStdString().c_str());
            j++;
        }
    }
    return res;
}



//----------------------------------------------------------------------------------------------------------------------
long double CalcParser::RoundS(long double arg, int precision)
{
    if(-1 == precision)
        return arg;
    String s = DoubleToString(arg, precision); // округление через строки...
    return ScaleToVal(s, scale);
}




//----------------------------------------------------------------------------------------------------------------------
String CalcParser::DoubleToString(long double n, int precision)
{
    String res;

    n = CheckNumberZero(n);

    if(10 == scale)
        res = String::number(n, 'g', precision);
    else
    {
        res = String::number((ulong)n, scale);
        res = res.toUpper();
    }
    return res;
}


//----------------------------------------------------------------------------------------------------------------------
Variable CalcParser::VariableByIterator(VarList::const_iterator vit)
{
    Variable v;
    v = vit->second;
    return v;
}


//----------------------------------------------------------------------------------------------------------------------
void CalcParser::InitMapToksHtml()
{
    ToksHtml["*"] = "&times;";
    ToksHtml["pi"] = "&pi;";
    ToksHtml["&"] = "&amp;";
    //ToksHtml["|"] = "&brvbar;";
    ToksHtml[">>"] = "&raquo;";
    ToksHtml["<<"] = "&laquo;";
    ToksHtml["sqrt"] = "&radic;";
    ToksHtml["terrt"] = "&sup3;&radic;";
    ToksHtml["%"] = "mod";
}


//----------------------------------------------------------------------------------------------------------------------
long double CalcParser::CheckNumberZero(long double n)
{
    if(fabs(n) < (1.0 * pow(10.0, -15)))
        return 0;
    return n;
}


//----------------------------------------------------------------------------------------------------------------------
const TokenList& CalcParser::RefTokens(void)
{
    return Tokens;
}


//----------------------------------------------------------------------------------------------------------------------
void CalcParser::InitFuncs()
{
    Function* f;
    for(auto i = 0; i < static_cast<int>(EFunc::user); ++i)
    {
        EFunc ef = static_cast<EFunc>(i);
        f = new Function(ef);
        map_funcs[f->getStrFunc()] = f;
    }
}
