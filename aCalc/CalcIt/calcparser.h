#ifndef CALCPARSER_H
#define CALCPARSER_H

#include <locale.h>
#include "consts.h"
#include "def.h"
#include "token.h"
#include "variable.h"
#include "cvalue.h"



class CalcParser
{
public:
    CalcParser(String *pexpr = NULL);
    ~CalcParser();
    long double GetResult();
    String GetResultStr();
    bool HasErr();
    const String listErrors();
    bool Run(void);
    void SetParams(String *pexpr, int scale = 10, int DRG_mode = RRAD);
    String GetExpression(String eq = "", bool html = false);
    bool AddToken(String *pexpr);
    bool AddPrefixOp(String op);
    bool AddPrefixInverse();
    Token* LastToken();
    String LastTokenValue();
    void ToBack(bool lastdigit = true);
    e_type_var TypeRes();
    void SetScale(int scale);
    int Scale();
    void SetDRG(int drg_mode);
    int DRG();
    String DoubleToString(long double n, int precision = 200);
    const TokenList& RefTokens(void);
    void SetVariable(String name, String value, e_type_var type = FLOAT, bool readonly = false);
    String GetStrValueVariable(String varname);


private:
    bool Calculate(CValue *loc_result);
    void GetToken();
    bool Space(Char c);
    bool strchr(String t, Char c);
    void Error(int c_err, Token *current_token = NULL);
    bool isdigit(Char c);
    bool isalpha(Char c);
    void Add_exp(CValue *res);
    void Mul_exp(CValue *res);
    void Step_exp(CValue *res);
    void Sign_exp(CValue *res);
    void Scob_exp(CValue *res);
    void GetNumber(CValue *res);
    long double ScaleToVal(String s, int scale = 10);
    unsigned FindFunction(const String *func, unsigned *n);
    long double CalcFunc(unsigned f, long double arg, unsigned n = 1, long double arg1 = 0);
    long double ChoiceArg(long double arg);
    long double ChoiceRes(long double arg);
    long double Factorial(long int arg);
    int IndexOf(const String* s, Char c);
    void ReadVariableToken(Token *loc_token);
    void LoadTokens();
    Token* LoadToken();
    bool InitVariableFromExpression();
    void InitExpr(String *pexpr);
    void EraseErrors();
    Variable VariableByIterator(VarList::const_iterator vit);
    int CheckParentheses();
    TokenList::iterator FindParentheses();
    void InitMapToksHtml();
    void InitHex2Bin();
    String HexToBinString(String s);
    long double CheckNumberZero(long double n);
    long double Dms(long double arg, bool invert);
    bool InvExpInNumber(Token* tok);
    long double RoundS(long double arg, int precision);

    CValue *result;
    String expr;
    Token *token;
    Token token_end;
    String::iterator exp_p;
    int err;
    int scale;
    int DRG_mode;
    lconv *lc;
    String dec_point;
    bool binit_var;
    String eq;

    TokenList Tokens;
    TokenList::iterator i_toks;

    VarList Vars;
    MapStrings ToksHtml;
    MapCharString Hex2Bin;

    double PI;

};

#endif // CALCPARSER_H
