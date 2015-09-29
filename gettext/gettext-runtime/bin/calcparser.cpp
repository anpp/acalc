#include "calcparser.h"


String _Errors[]={"Unknown error!", "Syntax error", "Unclosed parenthesis"
    ,"Unknown function", "Division by zero!",
                  "Invalid argument", "Value is too large", "Undefined variable"};


String Func[]={"sin(", "cos(", "tan(", "ctan(", "asin(", "acos(",
        "atan(", "actan(", "sinh(", "cosh(", "tanh(", "ctanh(", "asinh",
        "acosh(", "atanh(", "actanh(",
        "ln(", "log(", "fact("};

CalcParser::CalcParser(String *pexpr)
{
    Vars["pi"] = PI;
    Vars["e"] = exp(1);

    dec_point = "";
    token_end.SetType(END);
    SetParams(pexpr);
}


CalcParser::~CalcParser()
{
    Tokens.clear();
}


bool CalcParser::Calculate(void)
{
    result = 0;

    InitVariableFromExpression(); //если выражение содержит инициализацию переменных

    GetToken();
    if(token->Type() != END)
    {
        do{
            Add_exp(&result);
        }while(!(token->Type() == END || token->Value() == "="));
    }

    return !HasErr();
}


void CalcParser::InitVariableFromExpression()
{
    String var;
    Token t0, t1;
    Char c;

    TokenList::iterator ti = Tokens.begin();
    if(Tokens.size() > 1)
    {
        t0 = *ti++;
        t1 = *ti;
        if(t0.Type() == VARIABLE && t1.Value() == "=") //присвоение переменной
        {
            var = t0.Value();

            VarList::const_iterator var_it = Vars.find(var);
            if(var_it != Vars.end())
                return;

            binit_var = true;
            String::iterator it = expr.begin();
            do
            {
                c = *it;
                *it++ = ' ';
            }while (c != '=');
            LoadTokens();
            Calculate();
            if(!this->HasErr())
                Vars[var] = result;
        }
    }

}

void CalcParser::SetParams(String *pexpr, int scale, int DRG_mode)
{
    err = -1;
    binit_var = false;

    if(pexpr == NULL)
        expr = "";
    else
        expr = *pexpr;

    lc = localeconv();
    if(lc)
        dec_point.insert(0, lc->decimal_point);

    token = &token_end;
    this->scale = scale;
    this->DRG_mode = DRG_mode;
    LoadTokens();

}


bool CalcParser::HasErr()
{
    return err != -1;
}

double CalcParser::GetResult()
{
    return result;
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
        if(loc_token->Value() == "=")
            exp_p = expr.end();
    }

    else
    if(strchr("<>", *exp_p))
    {
        if(*exp_p == '<')
        {
            if(*(exp_p + 1) == '<')
                loc_token->Add(*exp_p);
        }
        else
        if(*exp_p == '>')
        {
            if(*(exp_p + 1) == '>')
                loc_token->Add(*exp_p);
        }

        loc_token->SetType(DELIMITER);
        if(!(*(loc_token->Value().end() - 1) == '>' || *(loc_token->Value().end() - 1) == '<'))
            Error(SYNTAX, loc_token);
        exp_p += 2;
    }
    else
        if(isalpha(*exp_p))
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
            if(!dec_point.empty())
            {
                if(*exp_p == '.' && dec_point != ".")
                    *exp_p = dec_point[0];
            }
            loc_token->Add(*exp_p++);
        }
        loc_token->SetType(NUMBER);
    }
    else
    {
        loc_token->Add(*exp_p++);
        loc_token->SetType(UNK);
        Error(SYNTAX, loc_token);
    }
    return loc_token;
}

void CalcParser::LoadTokens()
{
    Token *nTok;

    exp_p = expr.begin();
    Tokens.clear();

    while((nTok = LoadToken()) != NULL)
    {
        Tokens.push_back(*nTok);
        delete nTok;
    }
    i_toks = Tokens.begin();    
}



void CalcParser::GetToken()
{
    token = &token_end;
    if(i_toks == Tokens.end())
        return;
    token = &(*i_toks++);
    if(token->Type() == UNK)
        GetToken();
 }


String CalcParser::GetExpression()
{
    if(binit_var)
        return "";

    String es = "";
    Token t;
    TokenList::iterator tit = Tokens.begin();

    while(tit != Tokens.end())
    {
        t = *tit++;
        es.append(t.Value());
    }

    if(t.Value() != "=")
        es += "=";
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
        Tokens.push_back(t);
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
            errs.append(gettext(_Errors[t->Err()].toAscii()));
            errs += "\n";
        }
    }
    return errs;
}



bool CalcParser::isdigit(Char c)
{
    if(strchr("0123456789.,ie", c) || ((c >= 'A' && c <= 'F') && scale == 16))
        return true;
    return false;
}



bool CalcParser::isalpha(Char c)
{
    if((c >= 'a' && c <= 'z') || c == '_')
        return true;
    return false;
}


void CalcParser::Add_exp(double *res)
{
    double temp;
    Char op;

    Mul_exp(res);

    while(token->Value() == "+" || token->Value() == "-")
    {
        op = token->Value()[0];
        GetToken();
        Mul_exp(&temp);

        if(op == '+')
            *res += temp;
        if(op == '-')
            *res -= temp;
    }
}


void CalcParser::Mul_exp(double *res)
{
    double temp;
    Char op;
    char c_op;

    Step_exp(res);

    while(token->Value() == "*" || token->Value() == "/" || token->Value() == ">" ||
          token->Value() == "<" || token->Value() == "%" ||
        token->Value() == "&" || token->Value() == "|" || token->Value() == "!"
        || token->Value() == "(" || token->Type() == VARIABLE) //для неявного умножения без знака *
    {

        op = token->Value()[0];
#ifdef _QT4
        c_op = (char)(op.toAscii());
#else
        c_op = op;
#endif

        if(token->Value() == "(" || token->Type() == VARIABLE)
        {
            c_op = '*';
        }
        else
            GetToken();

        Step_exp(&temp);

        switch(c_op)
        {
        case '*':
            *res *= temp;
            break;
        case '/':
            if(temp == 0)
                Error(DIVISION);
            else
                *res /= temp;
            break;
        case '<':
            *res = (int)*res << (int)temp;
            break;
        case '>':
            *res = (int)*res >> (int)temp;
            break;
        case '%':
            if(temp == 0)
                Error(DIVISION);
            *res = fmod(*res, temp);
            break;
        case '&':
            *res = (int)*res & (int)temp;
            break;
        case '|':
            *res = (int)*res | (int)temp;
            break;
        case '!':
            *res = (int)*res ^(int)temp;
            break;
        }

    }
}


void CalcParser::Step_exp(double *res)
{
    double temp;

    Sign_exp(res);

    if((token->Type() == DELIMITER) && (token->Value() == "^"))
    {
        GetToken();
        Step_exp(&temp);
        *res = pow(*res, temp);
    }
}


void CalcParser::Sign_exp(double *res)
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
        *res = -*res;
    else
        if(op == '~')
            *res = ~(int)*res;
}

void CalcParser::Scob_exp(double *res)
{
    double temp;

    if(token->Type() == DELIMITER && token->Value() == "(")
    {
        GetToken();
        Add_exp(&temp);
        *res = temp;
        if(token->Value() != ")")
            Error(BKT);
        GetToken();
    }
    else
        GetNumber(res);
}


void CalcParser::GetNumber(double *res)
{
    double temp;
    int f;
    VarList::const_iterator var_it;

    switch(token->Type())
    {
    case NUMBER:
        {
        *res = ScaleToVal(token->Value(), scale);
        GetToken();
        break;
        }
    case FUNCTION:
        {
        f = FindFunction(&token->Value());
        GetToken();
        Add_exp(&temp);
        if(token->Value() != ")")
            Error(BKT);
        *res = CalcFunc(f, temp);
        GetToken();
        break;
        }
    case VARIABLE:
        var_it = Vars.find(token->Value());
        if(var_it != Vars.end())
        {
        #ifdef _QT4
            *res = *var_it;
        #else
            *res = var_it->second;
        #endif
        }
        else
            Error(UNKVAR);
        GetToken();
        break;
    default:
        Error(SYNTAX);
        GetToken();
    }
}



int CalcParser::FindFunction(const String *func)
{
int i;

for(i = 0; i < NUMFUNC; i++)
    if(func->compare(Func[i]) == 0)
            return i;

return -1;
}



double CalcParser::ScaleToVal(String s, int scale)
{
int len, i, j;
double res = 0;
String tst = "";
String::iterator val;

if(scale == 10)
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
    return res;
}
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
return res;
}


double CalcParser::CalcFunc(int f, double arg)
{
double res = 0;


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
                res = 1 / atanh(arg);
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
        default:
                Error(UNDEFINED);
        }
        return res;
}


double CalcParser::Factorial(long int arg)
{

if(arg == 0)
    return 1;
return Factorial(arg - 1) * arg;
}


double CalcParser::ChoiceArg(double arg)
{
long double temp = arg / (180 / PI);
if(DRG_mode == RDEG)
    return temp;
else
    if(DRG_mode == RGRAD)
        return  temp * 0.9;
return arg;
}


double CalcParser::ChoiceRes(double res)
{
double temp = (180 / PI) * res;

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
