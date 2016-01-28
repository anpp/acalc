#include "pnlbutton.h"


//----------------------------------------------------------------------------------------------------------------------
PnlButton::PnlButton(QWidget* parent)
{
    this->parent = parent;

    QFrame* w;
    for(auto pd: panelsDesc)
    {
        w = new QFrame(this->parent);
        w->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
        panelsW[pd.first] = w;
    }
}


//----------------------------------------------------------------------------------------------------------------------
PnlButton::~PnlButton()
{
    for (auto pd: panelsDesc) delete pd.second.color;
}



//----------------------------------------------------------------------------------------------------------------------
QFrame* PnlButton::getPanel(Pnl type)
{
    auto i = panelsW.find(type);
    return (i == panelsW.end()? nullptr: i->second);
}


//----------------------------------------------------------------------------------------------------------------------
PnlDesc* PnlButton::getPanelInfo(Pnl type)
{
    auto i = panelsDesc.find(type);
    return (i == panelsDesc.end()? nullptr: &i->second);
}


//----------------------------------------------------------------------------------------------------------------------
QColor* PnlButton::getColor(Pnl type)
{
    return getPanelInfo(type)->color;
}


//----------------------------------------------------------------------------------------------------------------------
int PnlButton::getRows(Pnl type)
{
    return getPanelInfo(type)->rows;
}


//----------------------------------------------------------------------------------------------------------------------
int PnlButton::getCols(Pnl type)
{
    return getPanelInfo(type)->cols;
}


//----------------------------------------------------------------------------------------------------------------------
void PnlButton::setSizeButton(Pnl type, int button_w, int button_h, int subw, int subh)
{
    int i_left, i_top, i_right, i_bottom;
    PnlDesc* pd = getPanelInfo(type);
    QFrame* panel = getPanel(type);
    if(!panel) return;

    i_left = i_right = i_top = i_bottom = 0;
    panel->getContentsMargins(&i_left, &i_top, &i_right, &i_bottom);

    panel->setFixedSize(button_w * pd->cols + pd->spacing * (pd->cols + 1) + i_left + i_right - subw,
                        button_h * pd->rows + pd->spacing * (pd->rows + 1) + i_top + i_bottom - subh);
}

