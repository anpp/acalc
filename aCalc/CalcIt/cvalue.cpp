#include "cvalue.h"

//*****************************************************************************************
CValue::CValue(e_type_var type)
{
    value = NULL;
    CreateValue(type);
}

void CValue::CreateValue(e_type_var type)
{
    if(value)
        DeleteValue();
    this->type = type;
    switch(type)
    {
    case FLOAT:
        value = (void*)new long double;
        *((long double*)(value)) = 0;
        break;
    case STRING:
        value = (void*)new String;
        break;
    default:
        break;
    }
}

CValue::~CValue()
{
    DeleteValue();
}

void CValue::DeleteValue()
{
    switch(type)
    {
    case FLOAT:
        delete (long double*)value;
        break;
    case STRING:
        delete (String*)value;
        break;
    default:
        break;
    }
    value = NULL;
}

bool CValue::SetValue(long double val)
{
    if(type == FLOAT)
    {
        *((long double*)(value)) = val;
        return true;
    }
    else
        return false;
}

bool CValue::SetValue(String val)
{
    if(type == STRING)
    {
        *((String*)(value)) = val;
        return true;
    }
    else
        return false;
}

long double CValue::ValueFloat()
{
    long double r = 0;
    if(type == FLOAT)
        r = *((long double*)value);
    return r;
}

String CValue::ValueString()
{
    String r = "";
    if(type == STRING)
        r = *((String*)value);
    return r;
}


void CValue::SetType(e_type_var new_type)
{
    DeleteValue();
    CreateValue(new_type);
}

e_type_var CValue::Type()
{
    return type;
}

//*****************************************************************************************
