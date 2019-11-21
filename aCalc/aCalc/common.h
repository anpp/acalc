#ifndef COMMON_H
#define COMMON_H


#define WIDTH_BUT  35
#define HEIGHT_BUT 35
#define SPACING 4
#define DEFAULT_DPI 96


enum consts: int {DIFF_HEIGHT = 10, MIN_HEIGHT = 5};

enum class Pnl {Dig, Op, Abc, Mem, /*MemDisp,*/ Func, Scale, Drg, FuncModes, ServButtons, Nop};

enum class ud {Result, Empty, Errors};

enum class SModes: int {Inv = 0, Hyp};
enum class Scales: int {Hex = 0, Dec, Oct, Bin};
enum class Serv: int {BackSpace = 0, CE, Esc};

enum class CalcView: int {Original = 0, Simple, Programmable};
enum class Langs: int {Nop = 0, En, Ru};
enum class Loging: int {Disable = 0, Enable};

#endif // COMMON_H
