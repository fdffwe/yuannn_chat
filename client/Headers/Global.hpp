// 定义一些常用的 头文件， 属性， 作用域
#pragma once
#include<QObject>
#include<QWidget>
#include<QStyle>

#include<functional>

#include<QNetworkReply>
#include<QNetworkAccessManager>
#include<QJsonDocument>
#include<QJsonObject>

/**
 * @brief 这个全局的 repolish 是用来刷新 qss 的新状态的
 * @note 每个对象的属性发生变化的时候，可以刷新，给我们显示
 */
extern std::function<void(QWidget*)> repolish;


enum ReqId{
    ID_GET_VERIFY_CODE = 1001,  // 获取验证码
    ID_REG_USER = 1002, // 注册用户
};

/**
 * @brief 模块区分：消息定义
 * @note 通过这个，可以把数据传输到： 对应的模块上去处理
 */
enum Modules{
    REGISTERMOD = 0,

};

enum ErrorCodes{
    // 内置实现同理 if(error) error 不能是 0 
    SUCCESS = 0,
    ERR_JSON =1, // json 解析失败
    ERR_NETWORK = 2,  // 网络错误


	UserExist = 1005,       //用户已经存在
};

enum TipErr{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VARIFY_ERR = 5,
    TIP_USER_ERR = 6
};

enum ClickLbState{
    Normal = 0,
    Selected = 1
};

extern QString GATE_URL_PREFIX;