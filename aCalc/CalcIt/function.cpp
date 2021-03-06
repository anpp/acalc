#include "function.h"


static QString funcs[] = {"sin(", "cos(", "tan(", "ctan(", "asin(", "acos(",
        "atan(", "actan(", "sinh(", "cosh(", "tanh(", "ctanh(", "asinh(",
        "acosh(", "atanh(", "actanh(",
        "ln(", "log(", "fact(", "max(", "min(", "dms(", "deg(", "sqrt(", "terrt(", "exp("};


static FuncOptions fos[] = {
    {EFunc::sin, 1, ETypeFunc::trig}, {EFunc::cos, 1, ETypeFunc::trig}, {EFunc::tan, 1, ETypeFunc::trig},
    {EFunc::ctan, 1, ETypeFunc::trig}, {EFunc::asin, 1, ETypeFunc::invtrig}, {EFunc::acos, 1, ETypeFunc::trig},
    {EFunc::atan, 1, ETypeFunc::invtrig}, {EFunc::actan, 1, ETypeFunc::invtrig}, {EFunc::sinh, 1, ETypeFunc::trig},
    {EFunc::cosh, 1, ETypeFunc::trig}, {EFunc::tanh, 1, ETypeFunc::trig}, {EFunc::ctanh, 1, ETypeFunc::trig},
    {EFunc::asinh, 1, ETypeFunc::invtrig}, {EFunc::acosh, 1, ETypeFunc::invtrig}, {EFunc::atanh, 1, ETypeFunc::invtrig},
    {EFunc::actanh, 1, ETypeFunc::invtrig}, {EFunc::ln, 1, ETypeFunc::regular}, {EFunc::log, 1, ETypeFunc::regular},
    {EFunc::fact, 1, ETypeFunc::regular}, {EFunc::max, 2, ETypeFunc::regular}, {EFunc::min, 2, ETypeFunc::regular},
    {EFunc::dms, 1, ETypeFunc::regular}, {EFunc::deg, 1, ETypeFunc::regular}, {EFunc::sqrt, 1, ETypeFunc::regular},
    {EFunc::terrt, 1, ETypeFunc::regular}, {EFunc::exp, 1, ETypeFunc::regular}
};

//----------------------------------------------------------------------------------------------------------------------
Function::Function(EFunc efunc): lfunc1(nullptr), lfunc2(nullptr)
{
    fo = fos[static_cast<int>(efunc)];
    switch(fo.efunc)
    {
    case EFunc::sin:
        lfunc1 = [] (double arg) { return sin(arg);};
        break;

    case EFunc::cos:
        lfunc1 = [] (double arg) { return cos(arg);};
        break;

    case EFunc::tan:
        lfunc1 = [] (double arg) { return tan(arg);};
        break;

    case EFunc::ctan:
        lfunc1 = [] (double arg) { return 1 / tan(arg);};
        break;

    case EFunc::asin:
        lfunc1 = [&] (double arg)
            { if(fabs(arg) > 1) {error_code = errors::INVALID; return 0.0;}
              return asin(arg);};
        break;

    case EFunc::acos:
        lfunc1 = [&] (double arg)
            { if(fabs(arg) > 1) {error_code = errors::INVALID; return 0.0;}
              return acos(arg);};
        break;

    case EFunc::atan:
        lfunc1 = [] (double arg) { return atan(arg);};
        break;

    case EFunc::actan:
        lfunc1 = [] (double arg) { return 1 / atan(arg);};
        break;

    case EFunc::sinh:
        lfunc1 = [] (double arg) { return sinh(arg);};
        break;

    case EFunc::cosh:
        lfunc1 = [] (double arg) { return cosh(arg);};
        break;

    case EFunc::tanh:
        lfunc1 = [] (double arg) { return tanh(arg);};
        break;

    case EFunc::ctanh:
        lfunc1 = [] (double arg) { return 1 / tanh(arg);};
        break;

    case EFunc::asinh:
        lfunc1 = [] (double arg) { return asinh(arg);};
        break;

    case EFunc::acosh:
        lfunc1 = [] (double arg) { return acosh(arg);};
        break;

    case EFunc::atanh:
        lfunc1 = [] (double arg) { return atanh(arg);};
        break;

    case EFunc::actanh:
        lfunc1 = [] (double arg) { return log((arg + 1) / (arg - 1)) / 2;};
        break;

    case EFunc::ln:
        lfunc1 = [] (double arg) { return log(arg);};
        break;

    case EFunc::log:
        lfunc1 = [] (double arg) { return log10(arg);};
        break;

    case EFunc::fact:
        lfunc1 = [&] (double arg)
            { if(arg < 0){error_code = errors::INVALID; return 0.0;}
              return static_cast<long int>(arg) == 0? 1: lfunc1(static_cast<long int>(arg - 1)) * static_cast<long int>(arg); };
        break;

    case EFunc::max:
        lfunc2 = [] (double arg1, double arg2) { return arg1 > arg2? arg1: arg2;};
        break;

    case EFunc::min:
        lfunc2 = [] (double arg1, double arg2) { return arg1 < arg2? arg1: arg2;};
        break;

    case EFunc::dms:
        lfunc1 = [&] (double arg) { return Dms(arg, false);};
        break;

    case EFunc::deg:
        lfunc1 = [&] (double arg) { return Dms(arg, true);};
        break;

    case EFunc::sqrt:
        lfunc1 = [&] (double arg)
            { if(arg < 0){error_code = errors::INVALID; return 0.0;}
              return sqrt(arg);};
        break;

    case EFunc::terrt:
        lfunc1 = [&] (double arg)
            { if(arg < 0){error_code = errors::INVALID; return 0.0;}
              return pow(arg, 1.0 / 3.0);};
        break;

    case EFunc::exp:
        lfunc1 = [] (double arg) { return exp(arg);};
        break;

    default:
        ;
    }
}


//----------------------------------------------------------------------------------------------------------------------
double Function::Dms(double arg, bool invert)
{
    double m = 0, s = 0, res;
    double koeff = 0.6;

    if(invert)
        koeff = 1;

    //45.1348

    m = modf(arg, &arg);
    m *= koeff * 100;
    s = modf(m, &m);

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
    return res;
}


//----------------------------------------------------------------------------------------------------------------------
const QString& Function::getStrFunc()
{
    return funcs[static_cast<int>(fo.efunc)];
}


//----------------------------------------------------------------------------------------------------------------------
double Function::ArgToRad(double arg, Drg DRG_mode)
{
    double temp = arg / (180 / acos(-1)) /*PI*/;
    if(DRG_mode == Drg::Deg)
        return temp;
    else
        if(DRG_mode == Drg::Grad)
            return  temp * 0.9;
    return arg;
}


//----------------------------------------------------------------------------------------------------------------------
double Function::RadToRes(double res, Drg DRG_mode)
{
    double temp = (180 / acos(-1) /*PI*/) * res;

    if(DRG_mode == Drg::Deg)
        return temp;
    else
        if(DRG_mode == Drg::Grad)
            return temp / 0.9;
    return res;
}


//----------------------------------------------------------------------------------------------------------------------
double Function::Calculate(double arg, Drg drg, Token* token)
{
    double result = 0;

    error_code = errors::SUCCESS;

    if(fo.t_func == ETypeFunc::trig)
        arg = ArgToRad(arg, drg);

    if (lfunc1 != nullptr)
    {
        result = lfunc1(arg);
        if (token && error_code != errors::SUCCESS)
            token->SetErr(error_code);
    }

    if(fo.t_func == ETypeFunc::invtrig)
        result = RadToRes(result, drg);
    return result;
}


//----------------------------------------------------------------------------------------------------------------------
double Function::Calculate(double arg1, double arg2, Token* token)
{
    double result = 0;

    error_code = errors::SUCCESS;

    if (lfunc2 != nullptr)
    {
        result = lfunc2(arg1, arg2);
        if (token && error_code != errors::SUCCESS)
            token->SetErr(error_code);
    }
    return result;
}
