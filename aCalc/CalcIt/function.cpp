#include "function.h"


QString funcs[] = {"sin(", "cos(", "tan(", "ctan(", "asin(", "acos(",
        "atan(", "actan(", "sinh(", "cosh(", "tanh(", "ctanh(", "asinh(",
        "acosh(", "atanh(", "actanh(",
        "ln(", "log(", "fact(", "max(", "min(", "dms(", "deg(", "sqrt(", "terrt(", "exp("};


FuncOptions fos[] = { {EFunc::sin, 1, ETypeFunc::trig}, {EFunc::cos, 1, ETypeFunc::trig}, {EFunc::tan, 1, ETypeFunc::trig},
                      {EFunc::ctan, 1, ETypeFunc::trig}, {EFunc::asin, 1, ETypeFunc::invtrig}, {EFunc::acos, 1, ETypeFunc::trig},
                      {EFunc::atan, 1, ETypeFunc::invtrig}, {EFunc::actan, 1, ETypeFunc::invtrig}, {EFunc::sinh, 1, ETypeFunc::trig},
                      {EFunc::cosh, 1, ETypeFunc::trig}, {EFunc::tanh, 1, ETypeFunc::trig}, {EFunc::ctanh, 1, ETypeFunc::trig},
                      {EFunc::asinh, 1, ETypeFunc::invtrig}, {EFunc::acosh, 1, ETypeFunc::invtrig}, {EFunc::atanh, 1, ETypeFunc::invtrig},
                      {EFunc::actanh, 1, ETypeFunc::invtrig}, {EFunc::ln, 1, ETypeFunc::regular}, {EFunc::log, 1, ETypeFunc::regular},
                      {EFunc::fact, 1, ETypeFunc::regular}, {EFunc::max, 2, ETypeFunc::regular}, {EFunc::min, 2, ETypeFunc::regular},
                      {EFunc::dms, 1, ETypeFunc::regular}, {EFunc::deg, 1, ETypeFunc::regular}, {EFunc::sqrt, 1, ETypeFunc::regular},
                      {EFunc::terrt, 1, ETypeFunc::regular}, {EFunc::exp, 1, ETypeFunc::regular}};

//----------------------------------------------------------------------------------------------------------------------
Function::Function(EFunc efunc): lfunc1(nullptr), lfunc2(nullptr)
{
    fo = fos[static_cast<int>(efunc)];
    switch(fo.efunc)
    {
    case EFunc::sin:
        lfunc1 = [] (long double arg) {return sin(arg);};
        break;
    case EFunc::cos:
        lfunc1 = [] (long double arg) {return cos(arg);};
        break;

    case EFunc::fact:
        lfunc1 = [&] (long double arg) { return (long int)arg == 0? 1: lfunc1((long int)arg - 1) * (long int)arg; };
        break;

    default:
        ;
    }
}


//----------------------------------------------------------------------------------------------------------------------
const QString& Function::getStrFunc()
{
    return funcs[static_cast<int>(fo.efunc)];
}


//----------------------------------------------------------------------------------------------------------------------
long double Function::ChoiceArg(long double arg, int DRG_mode)
{
long double temp = arg / (180 / acos(-1) /*PI*/);
if(DRG_mode == RDEG)
    return temp;
else
    if(DRG_mode == RGRAD)
        return  temp * 0.9;
return arg;
}


//----------------------------------------------------------------------------------------------------------------------
long double Function::ChoiceRes(long double res, int DRG_mode)
{
long double temp = (180 / acos(-1) /*PI*/) * res;

if(DRG_mode == RDEG)
    return temp;
else
    if(DRG_mode == RGRAD)
        return temp / 0.9;
return res;
}


//----------------------------------------------------------------------------------------------------------------------
long double Function::Calculate(long double arg, int drg)
{
    long double result = 0;

    if(fo.t_func == ETypeFunc::trig)
        arg = ChoiceArg(arg, drg);

    if (lfunc1 != nullptr)
        result = lfunc1(arg);

    if(fo.t_func == ETypeFunc::invtrig)
        result = ChoiceRes(result, drg);
    return result;
}


//----------------------------------------------------------------------------------------------------------------------
long double Function::Calculate(long double arg1, long double arg2, int drg)
{
    return 0;
}
