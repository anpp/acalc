#ifndef CONSTS_H
#define CONSTS_H

#include <math.h>


#define PRECISION_FOR_DOUBLE 12


enum class Drg {Deg, Rad, Grad};
enum class t_type {NOPE, ERR, NUMBER, VARIABLE, DELIMITER, FUNCTION, END, UNK, ENDLINE, TEXT, COMMA};
enum class errors: int {SUCCESS = -1, UNKNOWN = 0, SYNTAX, BKT, UNDEFINED, DIVISION, INVALID, SYST, UNKVAR, EMPTBKT, TMS, RO};
enum class e_type_var {FLOAT, STRING};

#endif // CONSTS_H
