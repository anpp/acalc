#ifndef CVALUE_H
#define CVALUE_H

#include "def.h"
#include "consts.h"

class CValue
{
public:
    CValue(e_type_var type = e_type_var::FLOAT);
    ~CValue();

    bool SetValue(double val);
    bool SetValue(String val);
    double ValueFloat();
    String ValueString();
    void SetType(e_type_var new_type);
    e_type_var Type();

private:
    void DeleteValue();
    void CreateValue(e_type_var type);

    void* value;
    e_type_var type;
};

#endif // CVALUE_H
