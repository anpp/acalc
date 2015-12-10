#include "qcalcwidget.h"

QCalcWidget::QCalcWidget(QWidget *widget, unsigned i, unsigned j, QString key, QString value, unsigned n_rows, unsigned n_cols, bool bsoftsize)
    :QObject(widget), widget(NULL), nopable(false), sReplaceValue(""), wasReplace(false)
{
    Init(widget, i, j, n_rows, n_cols, key, value);
    this->bsoftsize = bsoftsize;
    SetSize(WIDTH_BUT, HEIGHT_BUT);
}

QCalcWidget::QCalcWidget(QWidget *widget, unsigned i, unsigned j, pnl atype, QString key, QString value, unsigned n_rows, unsigned n_cols, bool bsoftsize)
    :QObject(widget), widget(NULL), nopable(false), sReplaceValue(""), wasReplace(false)
{    
    SetType(atype);
    Init(widget, i, j, n_rows, n_cols, key, value);    
    this->bsoftsize = bsoftsize;
    SetSize(WIDTH_BUT, HEIGHT_BUT);
}

void QCalcWidget::Init(QWidget *widget, unsigned i, unsigned j, unsigned n_rows, unsigned n_cols, QString key, QString value)
{
    QFont Font("arial", 10);

    this->lblW = NULL;
    this->widget = widget;
    this->i = i;
    this->j = j;
    this->n_rows = n_rows;
    this->n_cols = n_cols;
    this->sKeyText = key;
    this->sValueText = value;
    this->hyp = false;
    this->inv = false;
    this->hyp_mode = false;
    this->inv_mode = false;


    if(this->widget)
    {
        this->widget->setFont(Font);
        QString s = this->widget->metaObject()->className();
        if(s == "QPushButton")
        {
            if(key.indexOf("&") != -1 || key.indexOf("</") != -1)
            {
                QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight);
                layout->setMargin(0);
                layout->setSpacing(0);
                lblW = new QExLabel(this->widget);
                lblW->setFont(Font);
                lblW->setAlignment(Qt::AlignCenter);
                layout->addWidget(lblW);
                this->widget->setLayout(layout);

            }
            SetText(key);
            connect(widget, SIGNAL(clicked()), SLOT(sendSignalClick()));

        }
        else
            if(s == "QRadioButton" || s == "QCheckBox")
            {
                sKeyText = ((QAbstractButton*)(this->widget))->text();
                connect(widget, SIGNAL(clicked()), SLOT(sendSignalServClick()));
            }
    }
}


void QCalcWidget::SetText(const QString& text)
{
    if(widget)
    {
        if(lblW)
            lblW->setText("<CENTER>" + text + "</CENTER>");
        else
            ((QAbstractButton*)(this->widget))->setText(text);
    }
}

void QCalcWidget::LoadWhatIsText()
{
    if(!this->widget) return;
    if(!Value().isEmpty())
    {
        widget->setWhatsThis("<B></B>" + QObject::tr(Value().toStdString().c_str()));
        if(lblW)
            lblW->setWhatsThis("<B></B>" + QObject::tr(Value().toStdString().c_str()));
    }

}



void QCalcWidget::SetTextColor(QColor *color)
{
    if(color)
    {
        QPalette pal = widget->palette();
        pal.setColor(QPalette::ButtonText, *color);
        widget->setPalette(pal);
    }
}

void QCalcWidget::SetSize(unsigned w, unsigned h)
{
    if(bsoftsize)
        return;
    this->w = w;
    this->h = h;
    if(widget)
    {
        widget->setContentsMargins(0, 0, 0, 0);
        widget->setFixedSize(w, h);
    }
}


void QCalcWidget::SetAltTexts(QString altval, QString altkey)
{
    this->sAltKeyText = altkey;
    this->sAltValueText = altval;
}

void QCalcWidget::SetHypMode(bool mode)
{
    if(!hyp) return;
    hyp_mode = mode;
    if(hyp_mode && !sAltKeyText.isEmpty())
        SetText(sAltKeyText);
    else
       SetText(sKeyText);
}

void QCalcWidget::SetInvMode(bool mode)
{
    if(!inv) return;
    inv_mode = mode;
    if(inv_mode && !sAltKeyText.isEmpty())
        SetText(sAltKeyText);
    else
        SetText(sKeyText);
}

void QCalcWidget::SetType(pnl atype)
{
    type_btn = atype;
    if(atype != NOP) saved_type = atype;
    if(widget)
        widget->setVisible(atype != NOP);
}


void QCalcWidget::Replace(QCalcWidget* prev_widget)
{
    if(!prev_widget) return;
    wasReplace = true;
    saved_i = i;
    saved_j = j;
    saved_type = GetType();
    type_btn = prev_widget->GetType();
    i = prev_widget->i;
    j = prev_widget->j;
}

void QCalcWidget::UnReplace()
{
    if(!wasReplace) return;
    wasReplace = false;
    i = saved_i;
    j = saved_j;
    SetType(saved_type);
}
