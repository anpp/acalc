#ifndef QLOGCOMBOBOX_H
#define QLOGCOMBOBOX_H

#include <QWidget>
#include <QComboBox>
#include <QTableView>
#include <QHeaderView>
#include <QSizeGrip>

/*
class QLogSizeGrip: public QSizeGrip
{
    Q_OBJECT
    explicit QLogSizeGrip(QWidget *parent): QSizeGrip(parent){}
protected:
    void moveEvent(QMoveEvent *moveEvent)
    {
        this->
    }
};
*/

class QLogComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit QLogComboBox(QWidget *parent = nullptr);

protected:
    void showPopup()
    {
        emit OnPopup();
        //this->setItemData(0, Qt::AlignRight, Qt::TextAlignmentRole);
        QComboBox::showPopup();
        tv.setColumnWidth(0, this->width() - tv.verticalHeader()->width());
    }


private:
    QTableView tv;
    QSizeGrip sg;
signals:
    void OnPopup(void);
};

#endif // QLOGCOMBOBOX_H
