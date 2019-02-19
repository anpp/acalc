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
    unsigned int n_arguments;
    ETypeFunc t_func;
};

class Function
{
    FuncOptions fo;
    std::function<double (double)> lfunc1;
    std::function<double (double, double)> lfunc2;
    errors error_code;

    double ArgToRad(double arg, Drg DRG_mode);
    double RadToRes(double res, Drg DRG_mode);
    double Dms(double arg, bool invert);
public:
    Function(EFunc efunc);
    const QString& getStrFunc();
    double Calculate(double arg, Drg drg, Token* token = nullptr);
    double Calculate(double arg1, double arg2, Token* token = nullptr);
    unsigned int getCountAgrs() { return fo.n_arguments;}
};

#endif // FUNCTION_H
