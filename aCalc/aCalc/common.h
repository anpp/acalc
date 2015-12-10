#ifndef COMMON_H
#define COMMON_H

#define WIDTH_BUT  35
#define HEIGHT_BUT 35
#define SPACING 5


enum pnl{DIG = 0, OP, ABC, MEM, FUNC, SCALE, DRG, FUNCMODES, SERVBUTTONS, NOP};
enum ud {RES, RESEMPTY, ERRS};

enum SMODES {INV = 0, HYP};
enum SSCALES {SHEX = 0, SDEC, SOCT, SBIN};
enum SDRG {DDEG = 0, DRAD, DGRAD};
enum SSERV {BACKSPACE = 0, CE, ESC};

enum VIEWCALC {ORIGINAL = 0, SIMPLE, PROGRAMMABLE};
enum elangs {EN = 0, RU};

#endif // COMMON_H
