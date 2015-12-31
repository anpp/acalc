#include "calcparser.h"

#ifndef _QT4
    #include <sstream>
    #include <iomanip>

  #ifndef _QT_INTERFACE
    #define tr(text) gettext(text)
    #ifdef _UNIX
      #include </usr/local/include/libintl.h>
    #else
      #include "../../gettext/gettext-runtime/include/libintl.h"
    #endif
  #else
    //#include <QObject>
    //#define tr(text) QObject::tr(QString::fromStdString(text))
    #define tr(text) (text)
  #endif
#else
    #include <QObject>
    #define tr(text) QObject::tr(text)
#endif


String _Errors[] = {tr("Unknown error!"), tr("Syntax error"), tr("Unclosed parenthesis")
    ,tr("Unknown function"), tr("Division by zero!"),
                  tr("Invalid argument"), tr("Value is too large"), tr("Undefined variable"),
                  tr("Empty parentheses"), tr("Type mismatch"), tr("Readonly variable")};


String Func[] = {"sin(", "cos(", "tan(", "ctan(", "asin(", "acos(",
        "atan(", "actan(", "sinh(", "cosh(", "tanh(", "ctanh(", "asinh(",
        "acosh(", "atanh(", "actanh(",
        "ln(", "log(", "fact(", "max(", "min(", "dms(", "deg(", "sqrt(", "terrt(", "exp("};


String VarTypes[] = {"float", "string"};






CalcParser::CalcParser(String *pexpr): M_PI(acos(-1))
{
    result = new CValue(FLOAT);

    dec_point = "";
    eq = "";
    token_end.SetType(END);
    token_end.SetValue("END");
    SetParams(pexpr);
    InitMapToksHtml();

    SetVariable("pi", DoubleToString(M_PI), FLOAT, true);
    SetVariable("e", DoubleToString(exp(1)), FLOAT, true);
}


CalcParser::~CalcParser()
{
    Tokens.clear();
    delete result;
}


void CalcParser::SetVariable(String name, String value, e_type_var type, bool readonly)
{
#ifdef _QT4
    if(name.isEmpty() || name == "")
#else
    if(name.empty() || name == "")
#endif
        return;
    Variable var(name, value, type);
    var.read_only = readonly;
    Vars[var.name] = var;
}


bool CalcParser::Run(void)
{
    bool res = true;
    result->SetValue(0);
    i_toks = Tokens.begin();
    do
    {
        res &= Calculate(result);
    }while(token->Type() == ENDLINE);
    return res;
}

bool CalcParser::Calculate(CValue *loc_result)
{
    GetToken();
    while(token->Type() != ENDLINE && token->Type() != END)
    {
        result->SetType(FLOAT);
        Add_exp(loc_result);
        if(token->Type() != ENDLINE && token->Type() != END)
            Error(SYNTAX);

    }

    return !HasErr();
}


bool CalcParser::InitVariableFromExpression()
{
    Token t0, t1;
    Token *t = token;
    String var;
    TokenList::iterator it = i_toks;
    CValue res;
    Variable v;

    t0 = *token;
    if(t0.Type() == VARIABLE)
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
                        Error(RO);
                        return false;
                    }

                }

                v.type = t0.type_var;
                if(v.type != res.Type())
                {
                    Error(TMS);
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


void CalcParser::InitExpr(String *pexpr)
{
    binit_var = false;
    if(pexpr == NULL)
        expr = "";
    else
        expr = *pexpr;

    lc = localeconv();// надо перенести в расчет
    if(lc)
        dec_point.insert(0, lc->decimal_point);

    token = &token_end;
}

void CalcParser::SetParams(String *pexpr, int scale, int DRG_mode)
{
    err = -1;
    result->SetType(FLOAT);

    InitExpr(pexpr);

    SetScale(scale);
    SetDRG(DRG_mode);
    LoadTokens();

}

void CalcParser::SetScale(int scale)
{
    Token *t;
    long double number;
    String s;
    int prev_scale = this->scale;

    this->scale = scale;

    TokenList::iterator i = Tokens.begin();
    for(; i != Tokens.end(); ++i)
    {
        t = &(*i);
        if(t->Type() == NUMBER)
        {
            number = ScaleToVal(t->Value(), prev_scale);
            t->SetValue(DoubleToString(number));
        }
    }
}

int CalcParser::Scale()
{
    return scale;
}


void CalcParser::SetDRG(int drg_mode)
{
    if(drg_mode == 0)
        return;
    this->DRG_mode = drg_mode;
}

int CalcParser::DRG()
{
    return DRG_mode;
}


void CalcParser::LoadTokens()
{
    Token *nTok;

    exp_p = expr.begin();
    Tokens.clear();

    while((nTok = LoadToken()) != NULL)
    {
        if(nTok->Type() == VARIABLE)
        {
            VarList::const_iterator var_it = Vars.find(nTok->Value());
            if(var_it != Vars.end())
            {
                Variable v = VariableByIterator(var_it);
                nTok->type_var = v.type;
            }
        }
        if(nTok->Value() == ",")
            nTok->SetType(COMMA);
        Tokens.push_back(*nTok);
        delete nTok;
    }
}



int CalcParser::CheckParentheses()
{
   int res = 0;
   TokenList::iterator i = Tokens.begin();
   Token t;

   while(i != Tokens.end())
   {
       t = *i++;
       if(t.Value() == "(" || t.Type() == FUNCTION)
           res++;
       if(t.Value() == ")")
           res--;
   }
   return res;
}

TokenList::iterator CalcParser::FindParentheses()
{
    int res = 0;
    TokenList::iterator i = Tokens.end();
    Token t;

    if(Tokens.empty())
        return Tokens.end();

    while(i-- != Tokens.begin())
    {
        t = *i;

        if(t.Value() == "(" || t.Type() == FUNCTION)
            res++;
        if(t.Value() == ")")
            res--;
        if(0 == res)
            break;
    }
    if((t.Value() == "(" || t.Type() == FUNCTION) && res == 0)
        return i;
    return Tokens.end();
}


bool CalcParser::InvExpInNumber(Token* tok)
{
    int index;
    String value;

    if(!tok)
        return false;
    value = tok->Value();
    if(IndexOf(&value, 'e') == -1)
        return false;

    index = IndexOf(&value, '+');
    if(index != -1)
    {
        value[index] = '-';
        tok->SetValue(value);
        return true;
    }
    index = IndexOf(&value, '-');
    if(index != -1)
    {
        value[index] = '+';
        tok->SetValue(value);
        return true;
    }
    return false;
}



bool CalcParser::AddPrefixInverse()
{
    EraseErrors();

    Token *ltok = LastToken();
    Token before_tok;
    Token before_before_tok;
    TokenList::iterator i = Tokens.end();
    TokenList::iterator before_i;

        if(ltok->Type() == NUMBER || ltok->Value() == ")")
        {
            if(ltok->Type() == NUMBER)
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
                before_i = Tokens.insert(before_i, *(new Token("/", DELIMITER)));
                Tokens.insert(before_i, *(new Token("1", NUMBER)));
            }
            return true;
        }
        return false;
}



bool CalcParser::AddPrefixOp(String op)
{
    EraseErrors();

    Token *ltok = LastToken();
    Token before_tok;
    TokenList::iterator i = Tokens.end();
    TokenList::iterator before_i;

    if(op == "1/")
        return AddPrefixInverse();

    if(op == "-" || op == "~")
    {
        if(ltok->Type() == NUMBER || ltok->Value() == ")")
        {
            if(ltok->Type() == NUMBER)
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

                Tokens.insert(before_i, *(new Token(op, DELIMITER, true)));
                return true;
            }
        }
    }
    return false;
}



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

    while((nTok = LoadToken()) != NULL)
    {
        if(nTok->Value() == "del") //для отладки
        {
            ToBack();
        }
        else
        {
            ltok = LastToken();

            if(nTok->Type() == NUMBER)
            {
                if(nTok->Value() == "." && (ltok->Type() == END || ltok->Type() != NUMBER))
                    nTok->SetValue("0" + nTok->Value());

                if(ltok->Type() == NUMBER)
                {
                    if(IndexOf(&ltok->Value(), '.') != -1 && nTok->Value() == ".e+")
                        nTok->SetValue("e+");

                    if(IndexOf(&ltok->Value(), '.') != -1 && IndexOf(&nTok->Value(), '.') != -1)
                        return false;
                    if(IndexOf(&ltok->Value(), 'e') != -1 && IndexOf(&nTok->Value(), 'e') != -1)
                        return false;
                    if(IndexOf(&ltok->Value(), 'i') != -1 && IndexOf(&nTok->Value(), 'i') != -1)
                        return false;

                    ltok->Add(nTok->Value());
                }
                else
                {
                    if(!(ltok->Value() == ")" || ltok->Type() == VARIABLE))
                        Tokens.push_back(*nTok);
                    else
                        res = false;
                }
            }
            else
                if(nTok->Value() == "(")
                {
                    if(ltok->Type() != NUMBER && ltok->Type() != VARIABLE && ltok->Value() != ")")
                        Tokens.push_back(*nTok);
                    else
                        res = false;
                }
            else
                if(nTok->Value() == ")")
                {
                    if(ltok->Type() == NUMBER || ltok->Type() == VARIABLE || ltok->Value() == ")")
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
                if(nTok->Type() == FUNCTION || nTok->Type() == VARIABLE)
                {
                    if(ltok->Type() != NUMBER && ltok->Value() != ")" && ltok->Type() != VARIABLE)
                        Tokens.push_back(*nTok);
                    else
                        res = false;
                }
            else
                if(nTok->Type() == DELIMITER)
                {
                    if((ltok->Type() != DELIMITER && ltok->Type() != END && ltok->Type() != FUNCTION)
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

Token* CalcParser::LastToken()
{
    if(Tokens.empty())
        return &token_end;
    TokenList::iterator i = Tokens.end();
    Token *t = &(*--i);
    return t;
}

String CalcParser::LastTokenValue()
{
    return LastToken()->Value();
}


void CalcParser::ToBack(bool lastdigit)
{
    bool cutnumber = false;

    EraseErrors();
    Token *lastT = LastToken();

    if(lastT->Type() != END)
    {
        if(lastdigit)
            cutnumber = lastT->CutNumber();
        if(!cutnumber)
        {
            if(lastT->Value() == ")")
            {
                TokenList::iterator itP = FindParentheses();
                if(itP == Tokens.begin() && ((Token*)&(*itP))->Type() !=FUNCTION)
                    Tokens.pop_front();
            }

            Tokens.pop_back();
#ifdef _QT4
            i_toks = Tokens.end() - 1;
#endif
        }
    }
}


e_type_var CalcParser::TypeRes()
{
    return result->Type();
}

bool CalcParser::HasErr()
{
    return err != -1;
}

long double CalcParser::GetResult()
{
    return result->ValueFloat();
}

String CalcParser::GetResultStr()
{
    return result->ValueString();
}


void CalcParser::ReadVariableToken(Token *loc_token)
{
    while(isalpha(*exp_p) || strchr("1234567890", *exp_p))
        loc_token->Add(*exp_p++);
    if(*exp_p == '(')
    {
        loc_token->SetType(FUNCTION);
        loc_token->Add(*exp_p++);
    }
    else
        loc_token->SetType(VARIABLE);

}

Token* CalcParser::LoadToken()
{
    Token *loc_token;

    while(Space(*exp_p))
        ++exp_p;

    if (exp_p >= expr.end())
        return NULL;

    loc_token = new Token();

    if(strchr("*/+-()%^!&|~=", *exp_p))
    {
        loc_token->SetType(DELIMITER);
        loc_token->Add(*exp_p++);
        //if(loc_token->Value() == "=")
        //    exp_p = expr.end();
    }

    else
    if(strchr("<>", *exp_p))
    {
        loc_token->SetType(DELIMITER);
        if(*exp_p == '<' && *(exp_p + 1) == '<')
            loc_token->Add("<<");
        else
        if(*exp_p == '>' && *(exp_p + 1) == '>')
            loc_token->Add(">>");
        else
            Error(SYNTAX, loc_token);

        exp_p += 2;
    }
    else
        if(isalpha(*exp_p))
        {
            ReadVariableToken(loc_token);
            if(loc_token->Type() == VARIABLE)
            {
                if(loc_token->Value() == VarTypes[1]) //строковая переменная
                {
                    loc_token->SetValue("");
                    while(Space(*exp_p) && exp_p != expr.end())
                        ++exp_p;

                    if(isalpha(*exp_p))
                    {
                        ReadVariableToken(loc_token);
                        if(loc_token->Type() == VARIABLE)
                            loc_token->type_var = STRING;
                        else
                            Error(SYNTAX, loc_token);
                    }
                    else
                        Error(SYNTAX, loc_token);
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
                    Error(SYNTAX, loc_token);
            }
            loc_token->Add(*exp_p++);
        }
        loc_token->SetType(NUMBER);
    }
    else
    if(*exp_p == ';')
    {
        loc_token->Add(*exp_p++);
        loc_token->SetType(ENDLINE);
    }
    else
    if(*exp_p == '"')
    {
        ++exp_p;
        while(exp_p != expr.end() && *exp_p != '"')
            loc_token->Add(*exp_p++);

        if(*exp_p++ != '"')
            Error(SYNTAX, loc_token);

        loc_token->SetType(TEXT);
        loc_token->type_var = STRING;
    }
    else
    {
        loc_token->Add(*exp_p++);
        loc_token->SetType(UNK);
    }
    return loc_token;
}


void CalcParser::GetToken()
{
    token = &token_end;
    if(i_toks == Tokens.end())
        return;
    token = &(*i_toks++);
}


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

    TokenList::iterator tit = Tokens.begin();

    default_color = GetStrValueVariable("default_color");
    number_color = GetStrValueVariable("number_color");
    function_color = GetStrValueVariable("function_color");
    delimiter_color = GetStrValueVariable("delimiter_color");

    this->eq = eq;
    while(tit != Tokens.end())
    {
        t = *tit++;
        if(t.Type() != ERR)
        {
            tokVal = t.Value();

            b_tag = e_tag = "";

            space = " ";
            next_t = token_end;
            if(tit != Tokens.end())
            {
                next_t = *tit;
                if(next_t.Type() == ERR)
                    next_t.SetType(END);
            }

            if(html)
            {
                b_tag = "<FONT color=" + default_color + ">";
                if(t.Type() == NUMBER || t.prefix)
                    b_tag = "<FONT color=" + number_color + ">";
                if(t.Type() == FUNCTION)
                    b_tag = "<FONT color=" + function_color + ">";
                if(t.Type() == DELIMITER && t.Value() != ")" && t.Value() != "(" && !t.prefix)
                    b_tag = "<FONT color=" + delimiter_color + ">";
                e_tag = "</FONT>";

                if(t.Type() == FUNCTION)
#ifdef _QT4
                    tokVal.remove('(');
#else
                    tokVal.erase(tokVal.end() - 1);
#endif

                mit = ToksHtml.find(tokVal);
                if(mit != ToksHtml.end())
                {
#ifdef _QT4
                    tokVal = *mit;
#else
                    tokVal = mit->second;
#endif
                }
                if(t.Type() == FUNCTION)
                    tokVal += '(';
            }

                if(tokVal == "(" || next_t.Value() == ")" || t.Type() == FUNCTION || next_t.Type() == END || t.prefix)
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


bool CalcParser::Space(Char c)
{
    if(c == ' ' || c == ' ' || c == '\n') //пробел или таб или перевод строки
        return true;
    return false;
}


bool CalcParser::strchr(String t, Char c)
{
    if (IndexOf(&t, c) >= 0)
        return true;
    return false;
    }


void CalcParser::Error(int c_err, Token *current_token)
{
    Token t;

    err = c_err;
    if(!current_token)
        current_token = token;

    if(current_token->Type() == END)
    {
        t.SetType(ERR);
        t.SetErr(err);
        t.SetValue("ERR");

        Tokens.push_back(t);
        i_toks = Tokens.end();
    }
    else
        current_token->SetErr(err);
}

const String CalcParser::listErrors()
{
    String errs = "";
    Token *t;

    for(TokenList::iterator it = Tokens.begin(); it != Tokens.end(); it++)
    {
        t = &(*it);
        if(t->Err() != -1)
        {
            if((unsigned int)t->Err() < sizeof(_Errors)/sizeof(String))
                errs.append(tr(_Errors[t->Err()].toAscii()));
            else
                errs.append("?");
            errs += "\n";
        }
    }
    return errs;
}


void CalcParser::EraseErrors()
{
   // if(err == -1)
   //     return;

    Token *t;

    for(TokenList::iterator it = Tokens.begin(); it != Tokens.end(); ++it)
    {
        t = &(*it);

        if(t->Type() == ERR)
        {
            it = Tokens.erase(it);
            --it;
            continue;
        }
        if(t->Err() != -1)
            t->SetErr(-1);
    }
    err = -1;
}





bool CalcParser::isdigit(Char c)
{
    if((strchr("0123456789.ie", c) && scale == 10) || (((c >= 'A' && c <= 'F') || strchr("0123456789", c)) && scale == 16) ||
            (strchr("01", c) && scale == 2) || (strchr("01234567", c) && scale == 8) ||
            (((c >= 'A' && c <= 'F') || strchr("0123456789.e,", c)) && scale == 0))
        return true;
    return false;
}



bool CalcParser::isalpha(Char c)
{
    if((c >= 'a' && c <= 'z') || c == '_')
        return true;
    return false;
}


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
            Error(TMS);

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
#ifdef _QT4
    #ifdef HAVE_QT5
        c_op = (char)(op.toLatin1());
    #else
        c_op = (char)(op.toAscii());
    #endif
#else
        c_op = op;
#endif

        GetToken();
        Step_exp(&temp);

        if(temp.Type() != res->Type())
        {
            Error(TMS);
            break;
        }

        switch(c_op)
        {
        case '*':
            res->SetValue(RoundS((res->ValueFloat() * temp.ValueFloat()), PRECISION_FOR_DOUBLE));
            break;
        case '/':
            if(temp.ValueFloat() == 0)
                Error(DIVISION);
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
                Error(DIVISION);
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


void CalcParser::Step_exp(CValue *res)
{
    CValue temp;

    Sign_exp(res);

    if((token->Type() == DELIMITER) && (token->Value() == "^"))
    {
        GetToken();
        Step_exp(&temp);

        if(temp.Type() != res->Type())
            Error(TMS);

        res->SetValue(RoundS(pow(res->ValueFloat(), temp.ValueFloat()), PRECISION_FOR_DOUBLE));
    }
}


void CalcParser::Sign_exp(CValue *res)
{
    Char op = ' ';

    if((token->Type() == DELIMITER) &&
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

void CalcParser::Scob_exp(CValue *res)
{
    CValue temp;

    if(token->Type() == DELIMITER && token->Value() == "(")
    {
        GetToken();
        if(token->Value() == ")")
            Error(EMPTBKT);
        else
        {
            Add_exp(&temp);

            if(temp.Type() != res->Type())
                Error(TMS);

            res->SetValue(temp.ValueFloat());
            if(token->Value() != ")")
                Error(BKT);

        }
        GetToken();
        if(token->Type() == NUMBER)
            Error(SYNTAX);

    }
    else
        GetNumber(res);
}


void CalcParser::GetNumber(CValue *res)
{
    CValue temp;
    CValue temp1;
    unsigned f;
    unsigned f_n_par;
    String var;
    VarList::const_iterator var_it;
    Variable v;

    switch(token->Type())
    {
    case NUMBER:
        {
        res->SetValue(ScaleToVal(token->Value(), scale));
        GetToken();
        break;
        }
    case FUNCTION:
        {
        f = FindFunction(&token->Value(), &f_n_par);
        GetToken();
        Add_exp(&temp);
        switch(f_n_par)
        {
        case 1:
            if(token->Value() != ")")
                Error(BKT);
            res->SetValue(CalcFunc(f, temp.ValueFloat()));
            break;
        case 2:
            if(token->Type() != COMMA)
            {
                Error(SYNTAX);
                break;
            }
            GetToken();
            Add_exp(&temp1);
            if(token->Value() != ")")
            {
                Error(BKT);
                break;
            }
            res->SetValue(CalcFunc(f, temp.ValueFloat(), f_n_par, temp1.ValueFloat()));
            break;
        default:
            Error(UNKNOWN);
        }
        GetToken();
        break;
        }
    case VARIABLE:
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
            Error(UNKVAR);
        break;
    case TEXT:
        res->SetType(token->type_var);
        res->SetValue(token->Value());
        GetToken();
        break;
    default:
        Error(SYNTAX);
        GetToken();
    }
}


String CalcParser::GetStrValueVariable(String varname)
{
    VarList::const_iterator var_it = Vars.find(varname);
    Variable v;
    if(var_it != Vars.end())
    {
        v = VariableByIterator(var_it);
        return v.value;
    }
    return "";
}


unsigned CalcParser::FindFunction(const String *func, unsigned *n)
{
int i;

*n = 1;
for(i = 0; i < NUMFUNC; i++)
    if(func->compare(Func[i]) == 0)
    {
        if(i == FMAX || i == FMIN)
            *n = 2;
        return i;
    }

return -1;
}



long double CalcParser::ScaleToVal(String s, int scale)
{
    int len, i, j;
    long double res = 0;
    String tst = "";
    String::iterator val;
#ifdef _QT4
    if(!dec_point.isEmpty())
#else
    if(!dec_point.empty())
#endif
    {
        String::iterator it = s.begin();
        for(; it != s.end(); ++it)
            if(*it == '.' && dec_point != ".")
                *it = dec_point[0];
    }


    if(10 == scale)
    {
        if(IndexOf(&s, 'e') != -1)
        {
            val = s.begin() + IndexOf(&s, 'e') + 1;
            for(; val != s.end(); val++)
                tst += *val;
            s.resize(IndexOf(&s, 'e'));
            res = atof(s.toAscii()) * pow(10, atoi(tst.toAscii()));

        }
        else
            res = atof(s.toAscii());
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

            res += pow(scale, j) * atoi(tst.toAscii());
            j++;
        }
    }
    return res;
}


long double CalcParser::CalcFunc(unsigned f, long double arg, unsigned n, long double arg1)
{
long double res = 0;

        switch(f){
        case SIN:
                arg = ChoiceArg(arg);
                res = sin(arg);
                break;
        case COS:
                arg = ChoiceArg(arg);
                res = cos(arg);
                break;
        case TAN:
                arg = ChoiceArg(arg);
                res = tan(arg);
                break;
        case CTN:
                arg = ChoiceArg(arg);
                res = 1/tan(arg);
                break;
        case ARCSIN:
                if(fabs(arg) > 1)
                {
                    Error(INVALID);
                    break;
                }
                res = asin(arg);
                res = ChoiceRes(res);
                break;
        case ARCCOS:
                if(fabs(arg) > 1)
                {
                    Error(INVALID);
                    break;
                }
                res = acos(arg);
                res = ChoiceRes(res);
                break;
        case ARCTAN:
                res = atan(arg);
                res = ChoiceRes(res);
                break;
        case ARCCTN:
                res = 1 / atan(arg);
                res = ChoiceRes(res);
                break;
        case SINH:
                arg = ChoiceArg(arg);
                res = sinh(arg);
                break;
        case COSH:
                arg = ChoiceArg(arg);
                res = cosh(arg);
                break;
        case TANH:
                arg = ChoiceArg(arg);
                res = tanh(arg);
                break;
        case CTANH:
                arg = ChoiceArg(arg);
                res = 1 / tanh(arg);
                break;
        case ASINH:
                res = asinh(arg);
                res = ChoiceRes(res);
                break;
        case ACOSH:
                res = acosh(arg);
                res = ChoiceRes(res);
                break;
        case ATANH:
                res = atanh(arg);
                res = ChoiceRes(res);
                break;
        case ACTNH:
                res = log((arg + 1) / (arg - 1)) / 2;
                res = ChoiceRes(res);
                break;
        case LN:
                res = log(arg);
                break;
        case LOG:
                res= log10(arg);
                break;
        case FACT:
                if(arg < 0)
                {
                    Error(INVALID);
                    break;
                }
                res = Factorial((long int)arg);
                break;
        case FMAX:
                if(n != 2)
                {
                    Error(UNKNOWN);
                    break;
                }
                res = arg > arg1? arg: arg1;
                break;
        case FMIN:
                if(n != 2)
                {
                    Error(UNKNOWN);
                    break;
                }
                res = arg < arg1? arg: arg1;
                break;
        case DMS:
            res = Dms(arg, false);
            break;
        case DEG:
            res = Dms(arg, true);
            break;
        case SQRT:
            if(arg < 0)
            {
                Error(INVALID);
                break;
            }
            res = sqrt(arg);
            break;
        case TERRT:
            if(arg < 0)
            {
                Error(INVALID);
                break;
            }
            res = pow(arg, 1.0 / 3.0);
            break;
        case EXP:
            res = exp(arg);
            break;
        default:
                Error(UNDEFINED);
        }
        return CheckNumberZero(res);
}



long double CalcParser::Dms(long double arg, bool invert)
{
    long double m = 0, s = 0, res;
    long double koeff = 0.6;
    String temp;

    if(invert)
        koeff = 1;

    //45.1348

    m = modfl(arg, &arg);
    m *= koeff * 100;
    s = modfl(m, &m);

    if(s > 0.9999)
    {
        s = 0;
        m++;
    }

    s *= koeff;
    if(invert)
        res = arg + m / 60 + s / 36;
    else
        res = arg + (m + s) / 100;

    return RoundS(res, PRECISION_FOR_DOUBLE);
}


long double CalcParser::RoundS(long double arg, int precision)
{
    if(-1 == precision)
        return arg;
    String s = DoubleToString(arg, precision); // округление через строки...
    return ScaleToVal(s, scale);
}


long double CalcParser::Factorial(long int arg)
{

if(arg == 0)
    return 1;
return Factorial(arg - 1) * arg;
}


long double CalcParser::ChoiceArg(long double arg)
{
long double temp = arg / (180 / M_PI);
if(DRG_mode == RDEG)
    return temp;
else
    if(DRG_mode == RGRAD)
        return  temp * 0.9;
return arg;
}


long double CalcParser::ChoiceRes(long double res)
{
long double temp = (180 / M_PI) * res;

if(DRG_mode == RDEG)
    return temp;
else
    if(DRG_mode == RGRAD)
        return temp / 0.9;
return res;
}


int CalcParser::IndexOf(const String* s, Char c)
{
    Char ch = c;
#ifdef _QT4
    return s->indexOf(ch);
#else
    return s->find(ch);
#endif
}

String CalcParser::DoubleToString(long double n, int precision)
{
    String res;

    n = CheckNumberZero(n);

#ifdef _QT4
    if(10 == scale)
        res = String::number(n, 'g', precision);
    else
    {
        res = String::number((ulong)n, scale);
        res = res.toUpper();
    }
#else
    std::stringstream str_stream;

    if(10 == scale)
        str_stream << std::setprecision(precision) << n;
    if(16 == scale)
        str_stream << std::hex << (unsigned int)n;
    if(8 == scale)
        str_stream << std::oct << (unsigned int)n;
    if(2 == scale)
    {
        scale = 16;
        str_stream << HexToBinString(DoubleToString(n));
        scale = 2;
    }

    str_stream >> res;
    for (size_t i = 0; i < res.length(); ++i)
        res[i] = std::toupper(res[i]);

#endif
    return res;
}



Variable CalcParser::VariableByIterator(VarList::const_iterator vit)
{
    Variable v;
#ifdef _QT4
    v = *vit;
#else
    v = vit->second;
#endif
    return v;
}

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


void CalcParser::InitHex2Bin()
{
    Char Hex[] = {'0', '1', '2', '3', '4', '5',
                    '6', '7', '8', '9', 'A', 'B',
                    'C', 'D', 'E', 'F'};
    String Bin[] = {"0000", "0001", "0010", "0011", "0100", "0101",
                    "0110", "0111", "1000", "1001", "1010", "1011",
                    "1100", "1101", "1110", "1111"};
    if(Hex2Bin.empty())
    {
        for(int i = 0; i < 16; ++i)
            Hex2Bin[Hex[i]] = Bin[i];
    }

}


String CalcParser::HexToBinString(String s)
{
    InitHex2Bin();
    String result = "";
    Char h;
    String b;
    MapCharString::iterator mit;
    String::const_iterator it = s.begin();
    for(; it != s.end(); it++)
    {
        h = *it;
        mit = Hex2Bin.find(h);
        if(mit != Hex2Bin.end())
#ifdef _QT4
            b = *mit;
#else
            b = mit->second;
#endif
        else
            b = "";
        result.append(b);
    };
    return result;
}

long double CalcParser::CheckNumberZero(long double n)
{
    if(fabs(n) < (1.0 * pow(10.0, -15)))
        return 0;
    return n;
}

const TokenList& CalcParser::RefTokens(void)
{
    return Tokens;
}
