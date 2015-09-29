#include "variable.h"

Variable::Variable(String n, String v, e_type_var t)
{
    name = n;
    value = v;
    type = t;
    read_only = false;
}
