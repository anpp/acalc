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
