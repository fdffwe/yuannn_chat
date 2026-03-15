#pragma once
#include<QMainWindow>
#include <memory>

#include"Login.hpp"
#include"RegisterDialog.hpp"


namespace Ui{ class MainWindow; };


class MainWindow : public QMainWindow {
    Q_OBJECT // 使用信号与槽的必要声明， 还不能有 ； 

public:
    // 得这样写啊， 因为写了一个参数，就没有无参构造了 ，  这样的效果还是可以等效的
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    
public slots:
    void switchButtonClicked();




private:
    // 尝试智能指针： 自动析构， 不用自己管
    // share_ptr  unqiue_ptr
    // #include <memory>
    std::unique_ptr<Ui::MainWindow> _ui;
    Login *login_dialog = nullptr;
    RegisterDialog *register_dialog = nullptr;
};