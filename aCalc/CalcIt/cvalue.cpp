#include "cvalue.h"

//*****************************************************************************************
CValue::CValue(e_type_var type)
{
    value = nullptr;
    CreateValue(type);
}

void CValue::CreateValue(e_type_var type)
{
    if(value)
        DeleteValue();
    this->type = type;
    switch(type)
    {
    case e_type_var::FLOAT:
        value = (void*)new long double;
        *((long double*)(value)) = 0;
        break;
    case e_type_var::STRING:
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
    case e_type_var::FLOAT:
        delete (long double*)value;
        break;
    case e_type_var::STRING:
        delete (String*)value;
        break;
    default:
        break;
    }
    value = nullptr;
}

bool CValue::SetValue(long double val)
{
    if(type == e_type_var::FLOAT)
    {
        *((long double*)(value)) = val;
        return true;
    }
    else
        return false;
}

bool CValue::SetValue(String val)
{
    if(type == e_type_var::STRING)
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
    if(type == e_type_var::FLOAT)
        r = *((long double*)value);
    return r;
}

String CValue::ValueString()
{
    String r = "";
    if(type == e_type_var::STRING)
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
