#ifndef COMMON_H
#define COMMON_H


#define WIDTH_BUT  35
#define HEIGHT_BUT 35
#define SPACING 5
#define DEFAULT_DPI 96


enum class Pnl {Dig, Op, Abc, Mem, Func, Scale, Drg, FuncModes, ServButtons, Nop};

enum class ud {Result, Empty, Errors};

enum class SModes: int {Inv = 0, Hyp};
enum class Scales: int {Hex = 0, Dec, Oct, Bin};
enum class Drg: int {Deg = 0, Rad, Grad};
enum class Serv: int {BackSpace = 0, CE, Esc};

enum class CalcView: int {Original = 0, Simple, Programmable};
enum class Langs: int {En = 0, Ru};

#endif // COMMON_H
