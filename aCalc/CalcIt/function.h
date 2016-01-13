#ifndef FUNCTION_H
#define FUNCTION_H

#include <QString>
#include <functional>
#include "consts.h"

enum class EFunc: int {sin = 0, cos, tan, ctan, asin, acos, atan,
                  actan, sinh, cosh, tanh, ctanh, asinh, acosh, atanh, actanh,
                  ln, log, fact, max, min, dms, deg, sqrt, terrt, exp};

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

    long double ChoiceArg(long double arg, int DRG_mode);
    long double ChoiceRes(long double res, int DRG_mode);
public:
    Function(EFunc efunc);
    const QString& getStrFunc();
    long double Calculate(long double arg, int drg);
    long double Calculate(long double arg1, long double arg2, int drg);
    int getCountAgrs() { return fo.n_arguments;}
};

#endif // FUNCTION_H
