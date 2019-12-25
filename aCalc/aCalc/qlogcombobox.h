#ifndef QLOGCOMBOBOX_H
#define QLOGCOMBOBOX_H

#include <QWidget>
#include <QComboBox>
#include <QTableView>
#include <QHeaderView>
#include <QSizeGrip>
#include <QAbstractTableModel>
#include <QVector>


class LogTableModel : public QAbstractItemModel
{
    Q_OBJECT

    typedef QVector< std::pair< QString, QString > >  t_rows;
    t_rows   m_rows;
    int m_cols;
public:

    LogTableModel(QObject *parent = Q_NULLPTR)
        : QAbstractItemModel(parent), m_cols(1)
    {}


    QModelIndex index(int row, int column, const QModelIndex & = QModelIndex()) const
    {
        return createIndex(row, column);
    }

    QModelIndex parent(const QModelIndex &) const
    {
        return QModelIndex();
    }

    QVariant data(const QModelIndex &index, int role) const
    {
        if(role == Qt::DisplayRole && index.isValid())
             return m_rows.at(index.row()).second;
        return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const
    {
        if(role==Qt::DisplayRole && orientation==Qt::Vertical )
            return m_rows.at(section).first;
        return QVariant();
    }

    int rowCount(const QModelIndex&) const
    {
        return m_rows.size();
    }
    int columnCount(const QModelIndex&) const
    {
        return m_cols;
    }
    void addItems(const QStringList& texts)
    {
        for(auto s: texts)
        {
            QStringList sl = s.split("]");
            QString first = sl.at(0);
            QString second = sl.at(1);

            first = first.right(first.size() - 1);
            second = second.right(second.size() - 1);
            m_rows.push_back(std::make_pair(first, second));
        }
    }
    void clear()
    {
        m_rows.clear();
    }
};


class QLogTableView: public QTableView
{
    Q_OBJECT
protected:
    void resizeEvent(QResizeEvent *event)
    {
        this->QWidget::resizeEvent(event);
        setColumnWidth(0, width() - verticalHeader()->width());
    }
};


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
    }


private:
    QLogTableView tv;
    QSizeGrip sg;
signals:
    void OnPopup(void);
};

#endif // QLOGCOMBOBOX_H
