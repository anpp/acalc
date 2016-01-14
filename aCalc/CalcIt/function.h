#ifndef FUNCTION_H
#define FUNCTION_H

#include <QString>
#include <functional>
#include "consts.h"
#include "token.h"

enum class EFunc: int {sin = 0, cos, tan, ctan, asin, acos, atan,
                  actan, sinh, cosh, tanh, ctanh, asinh, acosh, atanh, actanh,
                  ln, log, fact, max, min, dms, deg, sqrt, terrt, exp, user};

enum class ETypeFunc {trig, invtrig, regular};

struct FuncOptions
{
    EFunc efunc;
    int n_arguments;
    ETypeFunc t_func;
};

class Function
{
    FuncOptions fo;
    std::function<long double (long double)> lfunc1;
    std::function<long double (long double, long double)> lfunc2;
    errors error_code;

    long double ArgToRad(long double arg, Drg DRG_mode);
    long double RadToRes(long double res, Drg DRG_mode);
    long double Dms(long double arg, bool invert);
public:
    Function(EFunc efunc);
    const QString& getStrFunc();
    long double Calculate(long double arg, Drg drg, Token* token = nullptr);
    long double Calculate(long double arg1, long double arg2, Token* token = nullptr);
    int getCountAgrs() { return fo.n_arguments;}
};

#endif // FUNCTION_H
