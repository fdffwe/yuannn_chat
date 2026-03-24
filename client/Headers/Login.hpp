#pragma once

#include<QDialog>
#include <memory>

#include "ClickedLabel.hpp"

namespace Ui{ class Login; };
// namespace Ui {
    // class Login;
// } // namespace Ui

class Login : public QDialog {
    Q_OBJECT // 使用信号与槽的必要声明， 还不能有 ； 

public:
    // 得这样写啊， 因为写了一个参数，就没有无参构造了 ，  这样的效果还是可以等效的
    explicit Login(QWidget *parent = nullptr);
    ~Login();


public slots:
    void slot_forget_pwd();


signals:
    void switchRegister();
    void switchReset();



private:
    // 尝试智能指针： 自动析构， 不用自己管
    // share_ptr  unqiue_ptr
    // #include <memory>
    std::unique_ptr<Ui::Login> _ui;
};