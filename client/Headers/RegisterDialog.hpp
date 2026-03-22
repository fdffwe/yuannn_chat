#pragma once
#include"Global.hpp"

#include<QDialog>
#include <memory>


namespace Ui{ class RegisterDialog; };


class RegisterDialog : public QDialog {
    Q_OBJECT // 使用信号与槽的必要声明， 还不能有 ； 

public:
    // 得这样写啊， 因为写了一个参数，就没有无参构造了 ，  这样的效果还是可以等效的
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

    // 提示语
    void showTip(QString str,bool isOk);

private:

    // 初始化： http请求的回调函数 
    // 在点击按钮的地方，发送 http 请求
    // 异步收到回复时， 统一在这里处理
    void initHttpHandlers();

    std::unique_ptr<Ui::RegisterDialog> _ui;
    QMap<ReqId,std::function<void(const QJsonObject&)>> _handlersMap;

public slots:

    /**
     * @brief 
     * @note 这是 Qt 的一种命名约定自动连接机制，当你按照特定格式命名槽函数时，Qt 会自动将信号连接到这个槽函数，无需手动调用 connect()。
     * @note 命名格式：on_<对象名>_<信号名>(<参数>)
     */
    void on_get_code_button_clicked();
    void on_sure_btn_clicked();


    void slot_reg_mod_finished(ReqId id ,QString res, ErrorCodes err);



};