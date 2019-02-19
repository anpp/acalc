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
        value = static_cast<void*>(new double);
        *(static_cast<double*>(value)) = 0;
        break;
    case e_type_var::STRING:
        value = static_cast<void*>(new String);
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
        delete static_cast<double*>(value);
        break;
    case e_type_var::STRING:
        delete static_cast<String*>(value);
        break;
    default:
        break;
    }
    value = nullptr;
}

bool CValue::SetValue(double val)
{
    if(type == e_type_var::FLOAT)
    {
        *(static_cast<double*>(value)) = val;
        return true;
    }
    else
        return false;
}

bool CValue::SetValue(String val)
{
    if(type == e_type_var::STRING)
    {
        *(static_cast<String*>(value)) = val;
        return true;
    }
    else
        return false;
}

double CValue::ValueFloat()
{
    double r = 0;
    if(type == e_type_var::FLOAT)
        r = *(static_cast<double*>(value));
    return r;
}

String CValue::ValueString()
{
    String r = "";
    if(type == e_type_var::STRING)
        r = *(static_cast<String*>(value));
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
