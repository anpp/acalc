#ifndef BUTTEXTS_H
#define BUTTEXTS_H

#include <QObject>
#include <QString>

QString sModes[] = {QObject::tr("Inv"), QObject::tr("Hyp")};
QString sScales[] = {QObject::tr("Hex"), QObject::tr("Dec"), QObject::tr("Oct"), QObject::tr("Bin")};
QString sDrg[] = {QObject::tr("Deg"), QObject::tr("Rad"), QObject::tr("Grad")};
QString sServ[] = {QObject::tr("Back"), QObject::tr("CE"), QObject::tr("ESC")};

QString sDigits[] = {"7", "8", "9", "4", "5", "6", "1", "2", "3", "0", ",", "+/-"};
QString svDigits[] = {QObject::tr("7"), QObject::tr("8"), QObject::tr("9"), QObject::tr("4"), QObject::tr("5"), QObject::tr("6"), QObject::tr("1"), QObject::tr("2"), QObject::tr("3"), QObject::tr("0"), QObject::tr("."), QObject::tr("+/-")};

QString sOperators[] = {"/", "Mod", "And", "*", "Or", "Xor", "-", "Lsh", "Rsh", "+", "=", "Not"};
QString svOperators[] = {QObject::tr("/"), QObject::tr("%"), QObject::tr("&"), QObject::tr("*"), QObject::tr("|"), QObject::tr("!"), QObject::tr("-"), QObject::tr("<<"), QObject::tr(">>"), QObject::tr("+"), QObject::tr("="), QObject::tr("~")};

QString sAbc[] = {QObject::tr("A"), QObject::tr("B"), QObject::tr("C"), QObject::tr("D"), QObject::tr("E"), QObject::tr("F")};


QString sMem[] = {QObject::tr("MS"), QObject::tr("MR"), QObject::tr("MC")};

QString sFunc[] = {"sin", "cos", "tan", "ctan", "ln", "log", "x&sup2;",
                   "asin", "acos", "atan", "actan", "exp", "10<sup>x</sup>", "x&sup3;",
                   "pi", "dms", "(", ")", "1/x", "n!", "x<sup>y</sup>"};
QString sAltFunc[] = {"sinh", "cosh", "tanh", "ctanh", "", "", "&radic;x",
                   "asinh", "acosh", "atanh", "actanh", "e", "", "&sup3;&radic;x",
                      "", "deg", "", "", "", "", "x<sup>(1/y)</sup>"};

QString svFunc[] = {QObject::tr("sin("), QObject::tr("cos("), QObject::tr("tan("), QObject::tr("ctan("), QObject::tr("ln("), QObject::tr("log("), QObject::tr("^2"),
                   QObject::tr("asin("), QObject::tr("acos("), QObject::tr("atan("), QObject::tr("actan("), QObject::tr("exp("), QObject::tr("exp"), QObject::tr("^3"),
                   QObject::tr("pi"), QObject::tr("dms("), QObject::tr("("), QObject::tr(")"), QObject::tr("1/x"), QObject::tr("fact("), QObject::tr("^")};

QString svAltFunc[] = {"sinh(", "cosh(", "tanh(", "ctanh(", "", "", "sqrt(",
                   "asinh(", "acosh(", "atanh(", "actanh(", "e", "", "terrt(",
                   "", "deg(", "", "", "", "", "^(1/"};

QString sHypInv[] = {"h", "h", "h", "h", "", "", "i",
                   "h", "h", "h", "h", "i", "", "i",
                   "", "i", "", "", "", "", "i"};


#endif // BUTTEXTS_H
