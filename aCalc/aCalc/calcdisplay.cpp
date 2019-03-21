#include "calcdisplay.h"

CalcDisplay::CalcDisplay(QWidget *parent) : QFrame(parent)
{
    wExpression = new QTextEdit(this);
    wResult = new QLabel(this);

    QFont dispFont("monospace", 11);
    wExpression->setFont(dispFont);
    wExpression->setAlignment(Qt::AlignRight);
    wExpression->setStyleSheet("QTextEdit {background-color: white}");
    wExpression->setReadOnly(true);
    wExpression->setFrameStyle(QFrame::NoFrame);
    wExpression->setLineWrapMode(QTextEdit::NoWrap);
    wExpression->setContentsMargins(0 ,0, 0, 0);
    wExpression->horizontalScrollBar()->setContentsMargins(0, 0, 0, 0);
    wExpression->setFixedHeight(wExpression->height() + wExpression->horizontalScrollBar()->height());
    wExpression->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    wExpression->setContextMenuPolicy(Qt::NoContextMenu);

    //dispFont.setPointSize(10);
    wResult->setFont(dispFont);
    wResult->setContentsMargins(0 ,0, 0, 0);
    wResult->setAlignment(Qt::AlignRight);
    wResult->setStyleSheet("QLabel {background-color: white}");
    wResult->setFixedHeight(wResult->height());
    this->setFixedHeight(wExpression->height() + wResult->height());
}

void CalcDisplay::Layout()
{
    QGridLayout *lDisplay = new QGridLayout();

    lDisplay->setMargin(0);
    lDisplay->setSpacing(0);
    lDisplay->addWidget(wExpression, 0, 0);
    lDisplay->addWidget(wResult, 1, 0);
    this->setLayout(lDisplay);
}

void CalcDisplay::setExpression(const QString &expression)
{
    wExpression->setText(expression);
    QTextCursor cursor = wExpression->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignRight);
    textBlockFormat.setIndent(0);
    cursor.mergeBlockFormat(textBlockFormat);
    cursor.movePosition(QTextCursor::End);

    wExpression->setTextCursor(cursor);

}

void CalcDisplay::setResult(const QString &result)
{
    wResult->setText(result);
}

void CalcDisplay::deleteLayout()
{
    delete this->layout();
}
