#pragma once 

#include <QDialog>
#include <QLabel>
#include <QList>
#include <QListWidgetItem>
#include <memory>
#include "Global.hpp"
// #include "statewidget.h"
// #include "statelabel.h"
// #include "userdata.h"

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    void ShowSearch(bool bsearch);
    void addChatUserList();

protected:

    
private:
    Ui::ChatDialog *ui;
    ChatUIMode _mode;
    ChatUIMode _state;
    bool _b_loading;
   
public slots:
    
private slots:

};

