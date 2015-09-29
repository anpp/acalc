#ifndef CONSTS_H
#define CONSTS_H

#include <math.h>


#define PRECISION_FOR_DOUBLE 15

//#define NUMTOK 200
#define NUMFUNC 26
#define MAXEXP 900
#define SIGN 15

#define DPOINT 11
#define NEG   12
#define BACK  14



//Идентификаторы служебных кнопок
#define BMUL  23
#define BDIV  20
#define BADD  29
#define BSUB  26
#define BSOL  30
#define BLSH  27
#define BRSH  28
#define BMOD  21
#define BNOT  31
#define BAND  22
#define BXOR  25
#define BOR   24

#define BCE   1000
#define BALL  1001

//Идентификаторы кнопок функций
//enum funct{BSIN = 60, BCOS, BTAN, BCTAN, BLN, BLOG, BX2, BARCSIN, BARCCOS, BARCTAN, BARCCTAN, BEX, B10X, BX3,
//         BPI, BDMS, BSC1, BSC2, B1DIVX, BFACT, BXY};



enum functions{SIN, COS, TAN, CTN, ARCSIN, ARCCOS, ARCTAN,
     ARCCTN, SINH, COSH, TANH, CTANH, ASINH, ACOSH, ATANH, ACTNH,
     LN, LOG, FACT, FMAX, FMIN, DMS, DEG, SQRT, TERRT, EXP};


#define BA 40
#define BB 41
#define BC 42
#define BD 43
#define BE 44
#define BF 45



#define BMS 50
#define BMR 51
#define BMC 52


enum rb1{RHEX=90, RDEC, ROCT, RBIN};
enum DRG{RDEG = 100, RRAD, RGRAD};
enum ch{CHBINV = 110, CHBHYP};
enum t_type{ERR = 1, NUMBER, VARIABLE, DELIMITER, FUNCTION, END, UNK, ENDLINE, TEXT, COMMA};
enum er{UNKNOWN = 0, SYNTAX, BKT, UNDEFINED, DIVISION, INVALID, SYST, UNKVAR, EMPTBKT, TMS, RO};
enum e_type_var{FLOAT=0, STRING};

#endif // CONSTS_H
