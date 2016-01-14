#ifndef VARIABLE_H
#define VARIABLE_H

#include "def.h"
#include "consts.h"


class Variable
{
public:
    Variable(String n = "", String v = "", e_type_var t = e_type_var::FLOAT);

    String name;
    String value;
    e_type_var type;
    bool read_only;
};

#endif // VARIABLE_H
