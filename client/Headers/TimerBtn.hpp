#pragma once 
#include <QPushButton>
#include <QTimer>



class TimerBtn : public QPushButton{
public:
    TimerBtn(QWidget *parent = nullptr);
    ~ TimerBtn();


    // 重写mouseReleaseEvent
    virtual void mouseReleaseEvent(QMouseEvent *e) override;
    
private:
    QTimer  *_timer;
    int _counter;
};