#include "pnlbutton.h"

PnlButton::PnlButton(std::initializer_list<PnlDesc> list)
{
    panels = list;
}


PnlButton::~PnlButton()
{
    for (PnlDesc pd: panels) delete pd.color;
}
