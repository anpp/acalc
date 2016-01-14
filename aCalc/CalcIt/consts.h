#ifndef CONSTS_H
#define CONSTS_H

#include <math.h>


#define PRECISION_FOR_DOUBLE 12



enum DRG{RDEG = 100, RRAD, RGRAD};

enum class t_type: int {NOPE, ERR, NUMBER, VARIABLE, DELIMITER, FUNCTION, END, UNK, ENDLINE, TEXT, COMMA};
enum class errors: int {SUCCESS = -1, UNKNOWN = 0, SYNTAX, BKT, UNDEFINED, DIVISION, INVALID, SYST, UNKVAR, EMPTBKT, TMS, RO};
enum e_type_var{FLOAT=0, STRING};

#endif // CONSTS_H
