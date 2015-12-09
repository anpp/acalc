#ifndef FUNCTORS
#define FUNCTORS

#include "common.h"
#include "qcalcwidget.h"


template<typename InputIterator, typename OutputIterator, typename Predicate>
OutputIterator copy_if(InputIterator begin, InputIterator end, OutputIterator destBegin, Predicate p)
{
    while(begin != end)
    {
        if(p(*begin)) *destBegin++ = *begin;
        ++begin;
    }
    return destBegin;
}



//FUNCTORS

class checkBtnType: public std::binary_function<QCalcWidget*, pnl, bool>{
public:
    bool operator()(QCalcWidget* btn, const pnl& atype) const
    {
        return btn->GetType() == atype;
    }
};


class buttonIsValue: public std::binary_function<QCalcWidget*, QString, bool>{
public:
    bool operator()(QCalcWidget* btn, const QString& value) const
    {
        return btn->Value() == value;
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


struct stTypeLayout {QLayout* l; pnl atype; bool bGrid;};

class setBtnLayout: public std::binary_function<QCalcWidget*, stTypeLayout, void>{
public:
    void operator()(QCalcWidget* btn, const stTypeLayout& TL) const
    {
        if(!TL.l) return;
        if(btn->GetType() == TL.atype)
        {
            if(TL.bGrid)
                ((QGridLayout*)(TL.l))->addWidget(btn->widget, btn->i, btn->j, btn->n_rows, btn->n_cols);
            else
                ((QBoxLayout*)(TL.l))->addWidget(btn->widget);
        }
    }

};


#endif // FUNCTORS

