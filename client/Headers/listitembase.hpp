#pragma once
#include <QWidget>
#include "Global.hpp"



class ListItemBase : public QWidget
{
    Q_OBJECT
public:
    explicit ListItemBase(QWidget *parent = nullptr);
    void SetItemType(ListItemType itemType);

    ListItemType GetItemType();
protected:
    void paintEvent(QPaintEvent* event);
private:
    ListItemType _itemType;

public slots:

signals:


};
