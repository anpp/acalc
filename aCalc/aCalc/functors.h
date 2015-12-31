#ifndef FUNCTORS
#define FUNCTORS

#include "common.h"
#include "qcalcwidget.h"


//FUNCTORS

class checkBtnType: public std::binary_function<QCalcWidget*, Pnl, bool>{
public:
    bool operator()(QCalcWidget* btn, const Pnl& atype) const
    {
        return btn->GetType() == atype;
    }
};


class buttonIsValue: public std::binary_function<QCalcWidget*, QString, bool>{
public:
    bool operator()(QCalcWidget* btn, const QString& value) const
    {
        return btn->Value() == value && value != "";
    }
};


struct stFuncMode {QString mode; bool on;};

class setHypInv: public std::binary_function<QCalcWidget*, stFuncMode, void>{
public:
    void operator()(QCalcWidget* btn, const stFuncMode& FM) const
    {
        if(btn->Hypable() && FM.mode == "Hyp")
            btn->SetHypMode(FM.on);
        if(btn->Invable() && FM.mode == "Inv")
            btn->SetInvMode(FM.on);

    }
};



#endif // FUNCTORS

