#pragma once
#include<QMainWindow>
#include <memory>

#include"Login.hpp"
#include"RegisterDialog.hpp"
#include "resetdialog.hpp"
#include "chatdialog.hpp"

namespace Ui{ class MainWindow; };


class MainWindow : public QMainWindow {
    Q_OBJECT // 使用信号与槽的必要声明， 还不能有 ； 

public:
    // 得这样写啊， 因为写了一个参数，就没有无参构造了 ，  这样的效果还是可以等效的
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    
public slots:
    void SlotSwitchReg();
    void SlotSwitchLogin();
    void SlotSwitchReset();
    void SlotSwitchLogin2();
    // chat 界面
    void SlotSwitchChat();

private:
    // 尝试智能指针： 自动析构， 不用自己管
    // share_ptr  unqiue_ptr
    // #include <memory>
    std::unique_ptr<Ui::MainWindow> ui;
    Login *_login_dlg;
    RegisterDialog *_reg_dlg = nullptr;
    ResetDialog* _reset_dlg = nullptr;
    ChatDialog* _chat_dlg = nullptr; 
};