#include "TimerBtn.hpp"
#include <QMouseEvent>
#include <QDebug>

// 计时器： 每 1 s emit clicked 信号，  counter --
// 这个就是显示的逻辑

TimerBtn::TimerBtn(QWidget *parent):QPushButton(parent),_counter(10){
    _timer = new QTimer(this);

    // 鼠标点击后， 会启动 timer 计时 1s， timeout 接收信号
    connect(_timer, &QTimer::timeout, [this](){
        _counter--;
        if(_counter <= 0){
            _timer->stop();
            _counter = 10;
            this->setText("获取");
            this->setEnabled(true);
            return;
        }
        this->setText(QString::number(_counter));
    });
}

TimerBtn::~TimerBtn(){
    _timer->stop();
}

void TimerBtn::mouseReleaseEvent(QMouseEvent *e){
    if (e->button() == Qt::LeftButton) {
        // 在这里处理鼠标左键释放事件
        qDebug() << "MyButton was released!";
        this->setEnabled(false);
        this->setText(QString::number(_counter));
        _timer->start(1000);

        // 干嘛用的：  给 error_tip 发送信号的( get_code_btn 自动连接的槽函数 )
        emit clicked();
    }
    // 调用基类的mouseReleaseEvent以确保正常的事件处理（如点击效果）
    QPushButton::mouseReleaseEvent(e);
}